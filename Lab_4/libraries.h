#pragma once

#include <iostream>
#include <string>
using namespace std;

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
CRITICAL_SECTION section_for_threads;
#define SLEEP_TIME 1
#define SLEEP_TIME_FOR_PRINT 40
#endif

#ifdef __linux__
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <linux/sched.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
struct termios orig_termios;
pthread_mutex_t * section_for_threads = new pthread_mutex_t();
#define Sleep usleep
#define SLEEP_TIME 1000
#define SLEEP_TIME_FOR_PRINT 40000
#endif

#define AMOUNT_OF_THREADS 10

static const char *name_of_thread[] = { "1.First", "2.Second",
"3.Third", "4.Fourth", "5.Fifth", "6.Sixth", "7.Seventh", "8.Eighth",
"9.Ninth", "10.Tenth", " thread\n", };