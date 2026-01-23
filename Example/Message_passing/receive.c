#include "msg.h"
#include <stdio.h>
#include <stddef.h>
#define BUFFER_SIZE 1000 
size_t received_len = 10;
int main (int argc, char* argv[]){
	char buffer[BUFFER_SIZE];
	while (1){
		int re_msg = msg_read("/my_queue", buffer, BUFFER_SIZE, (size_t*) &received_len);
		if (re_msg == MSG_SUCCESS){
			printf("Read process: %s\n", buffer);
		}
		else {
			perror ("Error when reading");
		}
	}
}
