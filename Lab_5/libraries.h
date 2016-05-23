#pragma once
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <process.h> 
#include <comdef.h>
#include <string>
#include<conio.h>

using namespace std;

#define FINISH_WRITE 2
#define FINISH_READ 0
#define EXIT 1
#define SIZE 100
#define MAX_STRING_SIZE 255

#elif linux
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <aio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#endif




