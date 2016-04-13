#include "libraries.h"
#include "client.h"
#include "server.h"

#ifdef _WIN32
int getch_noblock()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

void print_server() 
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cout << "Server: ";
	return;
}

void print_client() 
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cout << "Client: ";
	return;
}
#endif

#ifdef __linux__
char _getch() 
{
	char buf = 0;
	struct termios old = { 0 };
	fflush(stdout);
	if (tcgetattr(0, &old)<0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;   //Echo input characters.
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old)<0)  //The change occurs immediately
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1)<0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old)<0)	//The change occurs after all output written
		perror("tcsetattr ~ICANON");

	return buf;
}

int _kbhit(void) 
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}

int getch_noblock()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

char *itoa(int i) 
{
	char *buffer;
	buffer = (char *)calloc(12, sizeof(char));
	char *ptr = buffer + sizeof(buffer) - 1;
	unsigned int u;
	int minus = 0;

	if (i < 0) 
	{
		minus = 1;
		u = ((unsigned int)(-(1 + i))) + 1;
	}
	else 
	{
		u = i;
		*ptr = 0;
	}
	do {
		*--ptr = '0' + (u % 10);
	} while (u /= 10);

	if (minus)
		*--ptr = '-';
	return ptr;
}

void WaitSemaphore(int sem_id, int number) // wait while not 1 ->set 0
{
	struct sembuf buf;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_number = number;
	semop(sem_id, &buf, 1);
}

void ReleaseSemaphore(int sem_id, int number) // set 1
{
	struct sembuf buf;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_number = number;
	semop(sem_id, &buf, 1);
}
#endif

int main(int argc, char *argv[]) 
{
	if (argc == 1) 
	{
#ifdef _WIN32
		server Server(argv[0]);
		thread server_print(&server::server_print, Server);
		thread server_send(&server::server_send, Server);
		server_send.join();
		server_print.join();
		Server.stop_server();
#endif
#ifdef __linux__
		server Server;
		thread server_send(&server::server_send, Server, argv[0]);
		thread server_print(&server::server_print, Server);
		server_print.join();
		server_send.join();
		Server.stop_server();
#endif
	}
	else 
	{
#ifdef _WIN32
		client Client;
		thread client_print(&client::client_print, Client);
		thread client_send(&client::client_send, Client);
		client_send.join();
		client_print.join();
		Client.stop_client();
#endif
#ifdef __linux__
		client Client(argv[2], argv[3], argv[4], argv[5], argv[6]);
		thread client_print(&client::client_print, Client);
		thread client_send(&client::client_send, Client);
		client_send.join();
		client_print.join();
		Client.stop_client();
#endif
	}
	return 0;
}