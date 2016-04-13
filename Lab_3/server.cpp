#include "server.h"

#ifdef _WIN32
server::server(char *path) 
{
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ChildProcessInfo, sizeof(ChildProcessInfo));

	size_of_buffer = sizeof(buffer);

	Semaphore_client[0] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_CLIENT_START_PRINT");
	Semaphore_client[1] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_CLIENT_END_PRINT");
	Semaphore_client[2] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_CLIENT_PRINT_EXIT");
	Semaphore_client[3] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_CLIENT_SEND");
	Semaphore_client[4] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_CLIENT_SEND_EXIT");

	cout << "Client started..." << endl << endl;

	hNamedPipe = CreateNamedPipe("\\\\.\\pipe\\Pipe",
								PIPE_ACCESS_DUPLEX,
								PIPE_TYPE_MESSAGE | PIPE_WAIT,	// Synchronous messaging
								PIPE_UNLIMITED_INSTANCES,		// The maximum numberber of channels copies
								0,								// The size of the output bufferer 
								0, 
								INFINITE, 
								(LPSECURITY_ATTRIBUTES)NULL		// The default protection
								);

	CreateProcess(path, " 2", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &StartupInfo, &ChildProcessInfo);

	// Check 
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		cout << "Creation of the named pipe failed.\n";
	}

	if (!ConnectNamedPipe(hNamedPipe, (LPOVERLAPPED)NULL)) 
	{
		cout << "Connection fail.\n";
	}

	Semaphore_server[0] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_START_PRINT");
	Semaphore_server[1] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_END_PRINT");
	Semaphore_server[2] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_PRINT_EXIT");
	Semaphore_server[3] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_SEND");
	Semaphore_server[4] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE,"SEMAPHORE_SERVER_SEND_EXIT");
}

void server::server_send() 
{
	HANDLE Semaphores[2];
	Semaphores[0] = Semaphore_server[3];	//Semafore_Server_Send
	Semaphores[1] = Semaphore_server[4];	//Semafore_Server_Send_exit
	while (1) 
	{
		WaitForMultipleObjects(2, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;		//received permission

		ReleaseSemaphore(Semaphore_client[3], 1, NULL);		//SEMAPHORE_CLIENT_SEND
		if (getch_noblock() != -1) 
		{
			DWORD numberber_of_bytes_written;
			print_client();
			cin.clear();
			getline(cin, send_message);

			if (send_message == "end\0" || send_message == "exit\0") 
			{
				ReleaseSemaphore(Semaphore_server[2], 1, NULL);		//SEMAPHORE_SERVER_PRINT_EXIT
				ReleaseSemaphore(Semaphore_client[2], 1, NULL);		//SEMAPHORE_CLIENT_PRINT_EXIT
				ReleaseSemaphore(Semaphore_client[4], 1, NULL);		//SEMAPHORE_CLIENT_SEND_EXIT
				WaitForSingleObject(ChildProcessInfo.hProcess, INFINITE);
				break;
			}

			ReleaseSemaphore(Semaphore_client[0], 1, NULL);			//SEMAPHORE_CLIENT_START_PRINT

			int numberber_of_blocks = send_message.size() / size_of_buffer + 1;
			int size = send_message.size();
			WriteFile(hNamedPipe, &numberber_of_blocks, sizeof(numberber_of_blocks), &numberber_of_bytes_written, (LPOVERLAPPED)NULL);
			WriteFile(hNamedPipe, &size, sizeof(size), &numberber_of_bytes_written, (LPOVERLAPPED)NULL);
			for (int i = 0; i < numberber_of_blocks; i++) 
			{
				send_message.copy(buffer, size_of_buffer, i * size_of_buffer);
				if (!WriteFile(hNamedPipe, buffer, size_of_buffer, &numberber_of_bytes_written, (LPOVERLAPPED)NULL))
				{
					cout << "Write Error" << endl;
				}
			}	
			WaitForSingleObject(Semaphore_client[1], INFINITE);		//SEMAPHORE_CLIENT_END_PRINT
		}
		Sleep(1);
	}
}

void server::server_print() 
{
	HANDLE Semaphores[3];
	Semaphore_server[0] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_START_PRINT");
	Semaphore_server[1] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_END_PRINT");
	Semaphore_server[2] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_PRINT_EXIT");
	Semaphore_server[3] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_SEND");
	Semaphore_server[4] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_SERVER_SEND_EXIT");
	Semaphores[0] = Semaphore_server[0];	//SEMAPHORE_SERVER_START_PRINT
	Semaphores[1] = Semaphore_server[1];	//SEMAPHORE_SERVER_END_PRINT
	Semaphores[2] = Semaphore_server[2];	//SEMAPHORE_SERVER_PRINT_EXIT
	while (1) 
	{
		flag = TRUE;
		DWORD numberber_of_bytes_read;
		print_message.clear();
		int index = WaitForMultipleObjects(3, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (index == 2) break;
		
		int numberber_of_blocks;
		int size;
		if (!ReadFile(hNamedPipe, &numberber_of_blocks, sizeof(numberber_of_blocks), &numberber_of_bytes_read, NULL)) break;
		if (!ReadFile(hNamedPipe, &size, sizeof(size), &numberber_of_bytes_read, NULL)) break;
		for (int i = 0; i < numberber_of_blocks; i++) 
		{
			flag = ReadFile(hNamedPipe, buffer, size_of_buffer, &numberber_of_bytes_read, NULL);
			if (!flag) break;
			print_message.append(buffer, size_of_buffer);	
		}
		if (!flag) break;

		print_message.resize(size);
		print_server();
		for (int i = 0; i < size; i++) 
		{
			cout << print_message[i];
			Sleep(SLEEP_TIME);
		}
		cout << endl;
		ReleaseSemaphore(Semaphore_server[1], 1, NULL);		//SEMAPHORE_SERVER_END_PRINT
	}
}

void server::stop_server() 
{
	CloseHandle(hNamedPipe);
	for (int i = 0; i < 5; i++)
	{
		CloseHandle(Semaphore_client[i]);
		CloseHandle(Semaphore_server[i]);
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cout << endl;
	system("pause");
	return;
}
#endif

#ifdef __linux__

server::server() 
{
	size_of_buffer = BUFFER_SIZE;

	shm_id = shmget(IPC_PRIVATE, size_of_buffer, IPC_CREAT | 0666);  //new semafor
	if (shm_id < 0) 
	{
		printf("shmget error\n");
	}

	system("clear");

	sem_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
	semctl(sem_id, 0, SETALL, 0);
	if (sem_id < 0) 
	{
		printf("Semaphores is not created.");
	}
	sems_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
	semctl(sems_id, 0, SETALL, 0);
	if (sems_id < 0) 
	{
		printf("Semaphores is not created.");
	}
	sem_server_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	semctl(sem_server_id, 0, SETALL, 0);
	if (sem_server_id < 0) 
	{
		printf("Semaphores is not created.");
	}
	sem_client_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	semctl(sem_client_id, 0, SETALL, 0);
	if (sem_client_id < 0)
	{
		printf("Semaphores is not created.");
	}

	cout << "Client started..." << endl << endl;
	pid = fork();
}

void server::server_send(char *argv) 
{
	switch (pid)
	{
	case -1:
		printf("New process is not created\n");
		return;

	case 0:
		execlp("mate-terminal", "mate-terminal", "-x", argv, "-n 2", itoa(shm_id),
			itoa(sem_id), itoa(sems_id), itoa(sem_server_id), itoa(sem_client_id), NULL);
		break;

	default:
	{
		void *bufferer = shmat(shm_id, NULL, 0);
		while (1) 
		{
			system("stty -echo");
			ReleaseSemaphore(sem_client_id, 0);
			WaitSemaphore(sem_server_id, 0);
			if (getch_noblock() != -1) 
			{
				ReleaseSemaphore(sem_client_id, 1);
				send_message.clear();
				cout << "Client: ";
				system("stty echo");
				tcflush(STDIN_FILENO, TCIFLUSH);
				getline(cin, send_message);
				system("stty -echo");

				ReleaseSemaphore(sem_id, 2);

				if (send_message == "exit" || send_message == "end") 
				{
					length = -1;
					*(int*)bufferer = length;
					ReleaseSemaphore(sem_id, 0);
					ReleaseSemaphore(sems_id, 2);
					ReleaseSemaphore(sems_id, 0);
					ReleaseSemaphore(sem_client_id, 0);
					WaitSemaphore(sem_id, 1);
					waitpid(pid, NULL, 0);
					break;
				}

				length = send_message.size();
				*(int*)bufferer = length;

				ReleaseSemaphore(sem_id, 0);
				WaitSemaphore(sem_id, 1);

				numberberOfBlocks = ceil((double)length / (double)size_of_buffer);
				for (int i = 0; i < numberberOfBlocks; i++) 
				{
					send_message.copy((char*)bufferer, size_of_buffer, i*size_of_buffer);
					ReleaseSemaphore(sem_id, 0);
					WaitSemaphore(sem_id, 1);
				}
				WaitSemaphore(sem_id, 3);
			}
			usleep(SLEEP_TIME_ONE);
		}
	}
	break;
	}
}

void server::server_print() 
{
	void *bufferer = shmat(shm_id, NULL, 0);
	while (1) 
	{
		print_message.clear();

		WaitSemaphore(sems_id, 2);
		WaitSemaphore(sems_id, 0);

		length = *(int*)bufferer;
		ReleaseSemaphore(sems_id, 1);

		if (length == -1) break;

		numberberOfBlocks = ceil((double)length / (double)size_of_buffer);
		for (int i = 0; i < numberberOfBlocks; i++) 
		{
			WaitSemaphore(sems_id, 0);
			print_message.append((char*)bufferer, size_of_buffer);
			ReleaseSemaphore(sems_id, 1);
		}

		print_message.resize(length);
		cout << "Server: ";

		for (int i = 0; i < length; i++) 
		{
			putchar(print_message[i]);
			fflush(stdout);
			usleep(SLEEP_TIME);
		}
		cout << endl;
		ReleaseSemaphore(sems_id, 3);
	}
}

void server::stop_server() 
{
	system("clear");
	system("stty echo");
	return;
}
#endif