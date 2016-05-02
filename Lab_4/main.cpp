#include "libraries.h"
#include "threads.h"

int catch_input()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

int main(int argc, char *argv[]) {
	int number_of_thread = 0;
	int current_thread = 1;
	char choice = 'w';
	Thread *all_threads[AMOUNT_OF_THREADS];

	while (1) {		
		while (choice != 'q') {
			choice = catch_input();
			switch (choice) {
			case '+': {
						  if (number_of_thread < AMOUNT_OF_THREADS) {
							  number_of_thread++;
							  Thread *thread = new Thread(number_of_thread);
							  all_threads[number_of_thread - 1] = thread;
						  };
						  break;
			}
			case '-': {
						  if (number_of_thread > 0) {
							  all_threads[number_of_thread - 1]->end_of_print();
							  number_of_thread--;
						  };
						  break;
			}
			default: {
						 if (all_threads[current_thread]->can_print() && number_of_thread > 0) {
							 if (current_thread > number_of_thread) {
								 current_thread = 1;
							 }
							 all_threads[current_thread - 1]->start_of_print();
							 current_thread++;
						 }
						 break;
			}
			}
			Sleep(SLEEP_TIME);
		}

		for (int i = 0; i < number_of_thread; i++) {
			all_threads[i]->end_of_print();
		}
		exit(0);
	}
	return 0;
}

