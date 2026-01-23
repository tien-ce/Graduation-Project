#include "msg.h"
#include <mqueue.h>   // POSIX Message Queue functions
#include <fcntl.h>    // O_RDONLY, O_WRONLY, O_CREAT
#include <sys/stat.h> // S_IRUSR, S_IWUSR
#include <string.h>   // For strerror (though not strictly necessary for return values)
#include <stdio.h>    // For optional perror

// --- Function to set up default queue attributes (used by msg_read) ---
static void setup_default_attr(struct mq_attr *attr) {
    attr->mq_flags = 0;
    attr->mq_maxmsg = MAX_MSGS_DEFAULT;
    attr->mq_msgsize = MAX_MSG_SIZE_DEFAULT;
    attr->mq_curmsgs = 0;
}

// --- MSG_WRITE Implementation ---
int msg_write(const char *queue_name, const char *message, size_t msg_len) {
    mqd_t mq;
    int return_code = MSG_SUCCESS;
    // Open the queue for writing. It MUST exist already.
    mq = mq_open(queue_name, O_WRONLY);
    if (mq == (mqd_t)-1) {
        // If the queue doesn't exist, errno will be ENOENT
        perror("msg_write: mq_open failed");
        return MSG_E_QUEUE_OPEN;
    }

    // Check if the message exceeds the default max size (optional check, better done with proper queue attrs)
    if (msg_len > MAX_MSG_SIZE_DEFAULT) {
        fprintf(stderr, "msg_write: Message size %zu exceeds default maximum size %d\n", msg_len, MAX_MSG_SIZE_DEFAULT);
        // Note: For a strict library, you might not want to close on failure here.
    }
    
    // Send the message (priority set to 0)
    if (mq_send(mq, message, msg_len, 0) == -1) {
        perror("msg_write: mq_send failed");
        return_code = MSG_E_SEND_FAIL;
    }

    // Close the queue descriptor
    if (mq_close(mq) == -1) {
        perror("msg_write: mq_close failed");
        // Prioritize the send error if it occurred, otherwise return the close error
        if (return_code == MSG_SUCCESS) {
            return_code = MSG_E_QUEUE_CLOSE;
        }
    }

    return return_code;
}

// --- MSG_READ Implementation ---
int msg_read(const char *queue_name, char *buffer, size_t buffer_size, size_t *received_len) {
    mqd_t mq;
    struct mq_attr attr;
    ssize_t bytes_read;
    int return_code = MSG_SUCCESS;

    setup_default_attr(&attr);

    // Open the queue for reading. O_CREAT: Create if it doesn't exist.
    // Permissions 0644 (rw-r--r--). Attributes &attr are used ONLY if O_CREAT creates it.
    mq = mq_open(queue_name, O_RDONLY | O_CREAT, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("msg_read: mq_open failed");
        return MSG_E_QUEUE_OPEN;
    }

    // Check actual queue size against buffer size for safety
    if (mq_getattr(mq, &attr) == 0 && buffer_size < (size_t)attr.mq_msgsize) {
        fprintf(stderr, "msg_read: Warning: Buffer size %zu is smaller than queue's max message size %ld.\n", 
                buffer_size, attr.mq_msgsize);
        // Reception might still succeed for small messages, but buffer overflow is possible for large ones.
    }
    
    // Receive the message. Blocks until a message is available.
    bytes_read = mq_receive(mq, buffer, buffer_size, NULL);
    
    if (bytes_read == -1) {
        perror("msg_read: mq_receive failed");
        return_code = MSG_E_RECV_FAIL;
    } else {
        *received_len = (size_t)bytes_read;
    }

    // Close the queue descriptor
    if (mq_close(mq) == -1) {
        perror("msg_read: mq_close failed");
        if (return_code == MSG_SUCCESS) {
            return_code = MSG_E_QUEUE_CLOSE;
        }
    }

    // Note: mq_unlink is typically done by the process that created the queue or by a separate cleanup utility, 
    // not necessarily every time 'read' is called. It's omitted here for a reusable 'read' function.

    return return_code;
}

// --- MSG_CLEANUP Implementation ---
int msg_cleanup(const char *queue_name){
    if (mq_unlink(queue_name) == -1) {
        // If the queue doesn't exist (ENOENT), we might still treat it as success, 
        // but generally, we report the error.
        perror("msg_cleanup: mq_unlink failed"); 
        return MSG_E_GENERIC_FAIL; // Using a generic failure code for unlink errors
    }
    return MSG_SUCCESS;
}
