#include "msg.h"
#include <stddef.h>
#include <stdio.h>

int main (int argc, char* argv[]){
 	const char buffer[100] = "asdm\0";
	int msg_re = msg_write("/my_queue", buffer,(size_t)5);
	if (msg_re == MSG_SUCCESS) {
		printf ("From Send process: %s",buffer);
	}	
	return 0;
}
