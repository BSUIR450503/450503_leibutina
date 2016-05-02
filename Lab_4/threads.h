#include "libraries.h"

#ifdef __linux__
	void * print_thread(void *);
#elif _WIN32
	DWORD WINAPI print_thread(void *);
#endif

class Thread {
public:
	int current_thread;

#ifdef __linux__
	pthread_t *thread = new pthread_t();
	pthread_mutex_t *section_fot_print = new pthread_mutex_t();
	pthread_mutex_t *section_for_close = new pthread_mutex_t();
#endif

#ifdef _WIN32
	HANDLE thread_handle;
	CRITICAL_SECTION section_fot_print;
	CRITICAL_SECTION section_for_close;
	DWORD threadID;
#endif

	Thread(int & thread_number);
	bool can_print();
	bool pritn_end();
	bool close();
	void start_of_print();
	void print_end();
	void end_of_print();
};


Thread::Thread(int & thread_number) {
	current_thread = thread_number;
#ifdef _WIN32
	InitializeCriticalSection(&section_fot_print);
	InitializeCriticalSection(&section_for_close);
	if (thread_number == 1) {
		InitializeCriticalSection(&section_for_threads);
	}
	thread_handle = CreateThread(NULL, 100, print_thread, (void*)this, 0, &threadID);
}
#elif __linux__
	pthread_mutex_init(section_fot_print, NULL);
	pthread_mutex_lock(section_fot_print);
	pthread_mutex_init(section_for_close, NULL);
	if (thread_number == 1) {
		pthread_mutex_init(section_for_threads, NULL);
	}
	pthread_create(thread, NULL, print_thread, (void*)this);
}
#endif

bool Thread::can_print() {
#ifdef __linux__
	if (pthread_mutex_trylock(section_for_threads) != 0) return false;
	pthread_mutex_unlock(section_for_threads);
	return true;
#elif _WIN32
	if (TryEnterCriticalSection(&section_for_threads) == 0) return false;
	LeaveCriticalSection(&section_for_threads);
	return true;
#endif
}

void Thread::print_end() {
#ifdef __linux__
	pthread_mutex_lock(section_fot_print);
	pthread_mutex_unlock(section_for_threads);
#elif _WIN32
	LeaveCriticalSection(&section_fot_print);
	LeaveCriticalSection(&section_for_threads);
#endif
}

bool Thread::close() {
#ifdef __linux__
	if (pthread_mutex_trylock(section_for_close) != 0) return true;
	pthread_mutex_unlock(section_for_close);
	return false;
#elif _WIN32
	if (TryEnterCriticalSection(&section_for_close) == 0) return true;
	LeaveCriticalSection(&section_for_close);
	return false;
#endif
}

bool Thread::pritn_end() {
#ifdef __linux__
	if (pthread_mutex_trylock(section_fot_print) != 0) {
		return true;
	}
	pthread_mutex_unlock(section_fot_print);
	return false;
#elif _WIN32
	if (TryEnterCriticalSection(&section_fot_print) == 0) return false;
	LeaveCriticalSection(&section_fot_print);
	return true;
#endif
}

void Thread::start_of_print() {
#ifdef __linux__
	pthread_mutex_lock(section_for_threads);
	pthread_mutex_unlock(section_fot_print);
#elif _WIN32
	EnterCriticalSection(&section_fot_print);
#endif
}

void Thread::end_of_print() {
#ifdef __linux__
	pthread_mutex_lock(section_for_close);
#elif _WIN32
	EnterCriticalSection(&section_for_close);
#endif
}

#ifdef __linux__
char _getch() {
	char buf = 0;
	struct termios old = { 0 };
	fflush(stdout);
	if (tcgetattr(0, &old)<0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old)<0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1)<0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old)<0)
		perror("tcsetattr ~ICANON");

	return buf;
}

int _kbhit(void) {
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}
#endif


#ifdef __linux__
void *print_thread(void * name) {
#elif _WIN32
DWORD WINAPI print_thread(void * name) {
#endif
	Thread *thread = (Thread*)name;
	while (true) {
		if (!(thread->pritn_end())) {
#ifdef _WIN32
			EnterCriticalSection(&section_for_threads);
#endif
			for (int i = 0; name_of_thread[(thread->current_thread) - 1][i] != 0; i++) {
				printf("%c", name_of_thread[(thread->current_thread) - 1][i]);
				fflush(stdout);
				Sleep(SLEEP_TIME_FOR_PRINT);
			}
			for (int i = 0; name_of_thread[AMOUNT_OF_THREADS][i] != 0; i++) {
				printf("%c", name_of_thread[AMOUNT_OF_THREADS][i]);
				fflush(stdout);
				Sleep(SLEEP_TIME_FOR_PRINT);
			}
			fflush(stdout);
			thread->print_end();
		}
		else {
			Sleep(SLEEP_TIME);
		}
		if (thread->close()) {
			break;
		}
	}
	return 0;
}