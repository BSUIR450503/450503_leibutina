#pragma once
#ifdef _WIN32
#define _SCL_SECURE_NO_WARNINGS  
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <string>

#define BUFFER_SIZE 10
#define SLEEP_TIME 30
#define SIZE 5

void print_server();
void print_client();
#endif

#ifdef __linux__
#include <stdio.h>
#include <termios.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>
#include <math.h>

#define BUFFER_SIZE 10
#define SLEEP_TIME 30000
#define SLEEP_TIME_ONE 1000

char *itoa(int);
void WaitSemaphore(int, int);
void ReleaseSemaphore(int, int);
#endif

using namespace std;

int getch_noblock();