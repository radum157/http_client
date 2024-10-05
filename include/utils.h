#ifndef __UTILS_H
#define __UTILS_H 1

/* Connection data */
#define SERVER_HOST "34.246.184.49"
#define SERVER_PORT 8080

#define DIE(cond, msg) 						\
	do {									\
		if (cond) {							\
			perror(msg);					\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#endif
