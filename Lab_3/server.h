#pragma once
#include "libraries.h"

class server 
{
private:
#ifdef _WIN32
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ChildProcessInfo;
	HANDLE hNamedPipe;

	HANDLE Semaphore_client[SIZE];
	HANDLE Semaphore_server[SIZE];

	char buffer[BUFFER_SIZE];
	bool flag;
#endif

	int size_of_buffer;
	string send_message;
	string print_message;

#ifdef __linux__
	int pid;  
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
	server(char *);
	void server_send();
#endif

#ifdef __linux__
	server();
	void server_send(char *);
#endif

	void server_print();
	void stop_server();
};