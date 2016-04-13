#include "client.h"

#ifdef _WIN32
client::client() 
{
	size_of_buffer = sizeof(buffer);

	Semaphore_client[0] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_CLIENT_START_PRINT");
	Semaphore_client[1] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_CLIENT_END_PRINT");
	Semaphore_client[2] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_CLIENT_PRINT_EXIT");
	Semaphore_client[3] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_CLIENT_SEND");
	Semaphore_client[4] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_CLIENT_SEND_EXIT");

	Semaphore_server[0] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_SERVER_START_PRINT");
	Semaphore_server[1] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_SERVER_END_PRINT");
	Semaphore_server[2] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_SERVER_PRINT_EXIT");
	Semaphore_server[3] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_SERVER_SEND");
	Semaphore_server[4] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_SERVER_SEND_EXIT");

	cout << "Server started..." << endl << endl;

	hNamedPipe = CreateFile("\\\\.\\pipe\\Pipe", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}

void client::client_print() 
{
	HANDLE Semaphores[3];
	Semaphores[0] = Semaphore_client[0];		//SEMAPHORE_CLIENT_START_PRINT
	Semaphores[1] = Semaphore_client[1];		//SEMAPHORE_CLIENT_END_PRINT
	Semaphores[2] = Semaphore_client[2];		//SEMAPHORE_CLIENT_PRINT_EXIT
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

		print_client();
		for (int i = 0; i < size; i++)
		{
			cout << print_message[i];
			Sleep(SLEEP_TIME);
		}
		cout << endl;

		ReleaseSemaphore(Semaphore_client[1], 1, NULL);		//SEMAPHORE_CLIENT_END_PRINT
	}
}

void client::client_send() 
{
	HANDLE Semaphores[2];
	Semaphores[0] = Semaphore_client[3];	//SEMAPHORE_CLIENT_SEND
	Semaphores[1] = Semaphore_client[4];	//SEMAPHORE_CLIENT_SEND_EXIT
	while (1) 
	{
		ReleaseSemaphore(Semaphore_server[3], 1, NULL);		//SEMAPHORE_SERVER_SEND
		int index = WaitForMultipleObjects(2, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (index == 1) break;

		if (getch_noblock() != -1) 
		{
			DWORD numberber_of_bytes_written;
			print_server();
			cin.clear();
			getline(cin, send_message);

			ReleaseSemaphore(Semaphore_server[0], 1, NULL);		//SEMAPHORE_SERVER_START_PRINT

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
			WaitForSingleObject(Semaphore_server[1], INFINITE);		//SEMAPHORE_SERVER_END_PRINT
		}
		Sleep(1);
	}
}
#endif

void client::stop_client() 
{
#ifdef _WIN32
	CloseHandle(hNamedPipe);
	for (int i = 0; i < 5; i++)
	{
		CloseHandle(Semaphore_client[i]);
		CloseHandle(Semaphore_server[i]);
	}
#endif
	return;
}

#ifdef __linux__
client::client(char *argv_2, char *argv_3, char *argv_4,
	char *argv_5, char *argv_6) 
{
	size_of_buffer = BUFFER_SIZE;
	shm_id = atoi(argv_2);
	sem_id = atoi(argv_3);
	sems_id = atoi(argv_4);
	sem_server_id = atoi(argv_5);
	sem_client_id = atoi(argv_6);
	system("stty -echo");
	cout << "Server started..." << endl << endl;
}

void client::client_print()
{
	void *bufferer = shmat(shm_id, NULL, 0);
	while (1) {
		print_message.clear();

		WaitSemaphore(sem_id, 2);
		WaitSemaphore(sem_id, 0);

		length = *(int*)bufferer;
		ReleaseSemaphore(sem_id, 1);

		if (length == -1) break;

		numberberOfBlocks = ceil((double)length / (double)size_of_buffer);
		for (int i = 0; i < numberberOfBlocks; i++) 
		{
			WaitSemaphore(sem_id, 0);
			print_message.append((char*)bufferer, size_of_buffer);
			ReleaseSemaphore(sem_id, 1);
		}

		print_message.resize(length);
		cout << "Client: ";

		for (int i = 0; i < length; i++) 
		{
			putchar(print_message[i]);
			fflush(stdout);
			usleep(SLEEP_TIME);
		}
		cout << endl;

		ReleaseSemaphore(sem_id, 3);
	}
}

void client::client_send() 
{
	void *bufferer = shmat(shm_id, NULL, 0);
	while (1)
	{
		system("stty -echo");
		ReleaseSemaphore(sem_server_id, 0);
		WaitSemaphore(sem_client_id, 0);
		if (getch_noblock() != -1)
		{
			ReleaseSemaphore(sem_server_id, 1);
			send_message.clear();
			cout << "Server: ";
			system("stty echo");
			tcflush(STDIN_FILENO, TCIFLUSH);
			getline(cin, send_message);
			system("stty -echo");

			ReleaseSemaphore(sems_id, 2);

			length = send_message.size();
			*(int*)bufferer = length;

			ReleaseSemaphore(sems_id, 0);
			WaitSemaphore(sems_id, 1);

			numberberOfBlocks = ceil((double)length / (double)size_of_buffer);
			for (int i = 0; i < numberberOfBlocks; i++) 
			{
				send_message.copy((char*)bufferer, size_of_buffer, i*size_of_buffer);

				ReleaseSemaphore(sems_id, 0);
				WaitSemaphore(sems_id, 1);
			}
			WaitSemaphore(sems_id, 3);
		}
		usleep(SLEEP_TIME_ONE);
		length = *(int*)bufferer;
		if (length == -1) break;
	}
}
#endif