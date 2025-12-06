#ifndef MSG_LIB_H
#define MSG_LIB_H

#include <stddef.h> // For size_t

// --- Custom Error Codes for the Library ---
#define MSG_SUCCESS 0        // Task completed successfully
#define MSG_E_GENERIC_FAIL -1 // General failure (e.g., standard C library error)
#define MSG_E_QUEUE_OPEN -2  // Failed to open or create the message queue
#define MSG_E_SEND_FAIL -3   // Failed to send the message
#define MSG_E_RECV_FAIL -4   // Failed to receive the message
#define MSG_E_QUEUE_CLOSE -5 // Failed to close the queue descriptor

// --- Constants ---
#define QUEUE_NAME_DEFAULT "/task_queue"
#define MAX_MSG_SIZE_DEFAULT 256
#define MAX_MSGS_DEFAULT 10

/**
 * @brief Writes a message to the specified message queue.
 *
 * The queue must already exist. This function blocks if the queue is full.
 *
 * @param queue_name The name of the POSIX queue (e.g., "/my_queue").
 * @param message Pointer to the data buffer to send.
 * @param msg_len The size of the message in bytes.
 * @return MSG_SUCCESS (0) on success, or a negative error code on failure.
 */
int msg_write(const char *queue_name, const char *message, size_t msg_len);

/**
 * @brief Reads a message from the specified message queue.
 *
 * The function creates the queue if it does not exist using default attributes.
 * This function blocks if the queue is empty.
 *
 * @param queue_name The name of the POSIX queue (e.g., "/my_queue").
 * @param buffer Pointer to the buffer where the received message will be stored.
 * @param buffer_size The maximum size of the buffer.
 * @param received_len Pointer to a size_t where the actual received length will be stored.
 * @return MSG_SUCCESS (0) on success, or a negative error code on failure.
 */
int msg_read(const char *queue_name, char *buffer, size_t buffer_size, size_t *received_len);

/**
 * @brief Unlinks (deletes) the specified message queue from the system.
 *
 * This function should be called once after communication is complete to clean up the resource.
 *
 * @param queue_name The name of the POSIX queue (e.g., "/my_queue").
 * @return MSG_SUCCESS (0) on success, or a negative error code on failure.
 */
int msg_cleanup(const char *queue_name);
#endif // MSG_LIB_H