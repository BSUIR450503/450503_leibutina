#pragma once
#include "libraries.h"

class client 
{
private:
#ifdef _WIN32
	HANDLE hNamedPipe;

	HANDLE Semaphore_client[SIZE];
	HANDLE Semaphore_server[SIZE];

	char buffer[BUFFER_SIZE];
	bool flag;
#endif

	int size_of_buffer;
	string print_message;
	string send_message;

#ifdef __linux__
	int shm_id;
	int sem_id;
	int sems_id;
	int sem_server_id;
	int sem_client_id;
	int numberberOfBlocks;
	int length;
#endif

public:
#ifdef _WIN32
	client();
#endif

#ifdef __linux__
	client(char *, char *, char *, char *, char *);
#endif

	void stop_client();
	void client_print();
	void client_send();
};