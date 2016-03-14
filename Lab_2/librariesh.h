#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include<stdlib.h>
#define amount 11

#ifdef _WIN32
#include <windows.h>
#include<conio.h>
#endif

#ifdef linux
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SLEEP_TIME 50000
#define SLEEP_TIME_ONE 1000
#define PROCESS_INFORMATION pid_t
#endif

using namespace std;

