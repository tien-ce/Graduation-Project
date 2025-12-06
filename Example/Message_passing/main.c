#include <stdio.h>    // For standard I/O functions like printf, perror
#include <stdlib.h>   // For general utilities like exit, EXIT_FAILURE
#include <string.h>   // For string manipulation functions like strlen
#include <fcntl.h>    // For O_CREAT, O_EXCL (file control options)
#include <sys/stat.h> // For S_IRUSR, S_IWUSR (file permission macros)
#include <mqueue.h>   // **Crucial: For POSIX message queue functions (mq_open, mq_send, etc.)**
#include <unistd.h>   // For fork
#include <sys/wait.h> // Wait functions
#define QUEUE_NAME "/my_message_queue" // Define the name for the message queue (must start with /)
#define MAX_SIZE 256                 // Define the maximum size (in bytes) of a single message

int main() {
    mqd_t mq;                       // Message queue descriptor (like a file descriptor for the queue)
    struct mq_attr attr;            // Structure to define message queue attributes
    char buffer[MAX_SIZE];          // Buffer to hold the message data (both sending and receiving)

    // Set up message queue attributes
    attr.mq_flags = 0;              // Set flags (0 means blocking queue operations by default)
    attr.mq_maxmsg = 10;            // Maximum number of messages the queue can hold
    attr.mq_msgsize = MAX_SIZE;     // Maximum size of a single message (must match the buffer size)
    attr.mq_curmsgs = 0;            // Current number of messages in the queue (ignored when opening)

    pid_t pid = fork();             // **Create a new child process**

    if (pid == -1) {                // Check if fork failed
        perror("fork");             
        exit(EXIT_FAILURE);         // Exit with failure status
    }

    if (pid == 0) { // Child process (receiver)
        // Open message queue for reading
        // O_RDONLY: Open for reading. O_CREAT: Create the queue if it doesn't exist.
        // 0644: Permissions (rw-r--r--). &attr: Queue attributes (only used if O_CREAT is set).
        mq = mq_open(QUEUE_NAME, O_RDONLY | O_CREAT, 0644, &attr);
        if (mq == (mqd_t)-1) {
            perror("mq_open (receiver)");
            exit(EXIT_FAILURE);
        }

        printf("Receiver: Waiting for messages...\n");
        // Block and receive a message from the queue.
        // buffer: Destination. MAX_SIZE: Max bytes to read. NULL: Priority (not used here).
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        if (bytes_read == -1) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_read] = '\0'; // **Add null-terminator** to treat the received data as a C string
        printf("Receiver: Received message: %s\n", buffer);

        mq_close(mq);            // Close the message queue descriptor
        mq_unlink(QUEUE_NAME);   // **Unlink/Delete the message queue** from the system (necessary for cleanup)
    } else { // Parent process (sender)
        // Open message queue for writing (Similar to the child, but with O_WRONLY)
        mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, 0644, &attr);
        if (mq == (mqd_t)-1) {
            perror("mq_open (sender)");
            exit(EXIT_FAILURE);
        }

        const char *message = "Hello from the sender!"; // The message content to send
        printf("Sender: Sending message: %s\n", message);
        // Send the message to the queue.
        // message: Source. strlen(message): Number of bytes to send. 0: Priority (not used here).
        if (mq_send(mq, message, strlen(message), 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }

        mq_close(mq);            // Close the message queue descriptor
        wait(NULL);              // **Wait for the child process to finish** before the parent exits
    }

    return 0; // Exit successfully
}