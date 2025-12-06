#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // For fork
#include <sys/wait.h>   // For wait

// Include our custom message queue library
#include "msg.h"    

// We still define the name and size, but now they align with the library's defaults
#define QUEUE_NAME QUEUE_NAME_DEFAULT // Defined in msg_lib.h as "/task_queue"
#define MAX_SIZE MAX_MSG_SIZE_DEFAULT // Defined in msg_lib.h as 256

// Helper function to interpret library error codes
void check_error_and_exit(int status, const char* context) {
    if (status != MSG_SUCCESS) {
        fprintf(stderr, "%s failed with error code: %d\n", context, status);
        // We can add a switch statement here to print detailed error messages if needed.
        exit(EXIT_FAILURE);
    }
}

int main() {
    char buffer[MAX_SIZE];
    size_t received_len;
    int status;
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process (receiver)
        printf("Receiver: Waiting for messages on %s...\n", QUEUE_NAME);
        
        // **Library Call:** Read the message. msg_read will create the queue if it doesn't exist.
        status = msg_read(QUEUE_NAME, buffer, MAX_SIZE, &received_len);
        check_error_and_exit(status, "msg_read");

        // The library already read the data; now we just use the received_len
        buffer[received_len] = '\0';
        printf("Receiver: Received message: %s (Length: %zu)\n", buffer, received_len);

        // Cleanup: Call the library function to remove the queue
        status = msg_cleanup(QUEUE_NAME);
        check_error_and_exit(status, "msg_cleanup");

    } else { // Parent process (sender)
        const char *message = "Hello from the sender!";
        printf("Sender: Sending message: %s to %s\n", message, QUEUE_NAME);
        
        // **Library Call:** Write the message.
        status = msg_write(QUEUE_NAME, message, strlen(message));
        check_error_and_exit(status, "msg_write");

        // Wait for the child process to finish
        wait(NULL);
    }

    return 0;
}