#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#define MAX_MEMORY_SIZE 100

using namespace std;

typedef struct memory_block
{
	uint8_t is_free;
	uint8_t index;
	uint8_t length;
	uint8_t* mem_pointer;
	struct memory_block *next;
	struct memory_block *prev;
} mblock_t;

typedef struct __pointer__
{
	uint8_t is_busy;
	mblock_t* block;
} memory_pointer;

extern memory_pointer pointers[MAX_MEMORY_SIZE];

void init_memory();
mblock_t* malloc_memory(uint8_t size);
mblock_t* realloc_memory(void* ptr, uint8_t size);
void free_memory(void* ptr);
void free_all_memory();
void state();
void view_memory();
void defragmentation();