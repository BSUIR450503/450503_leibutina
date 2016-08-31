#pragma once
#include <iostream>

class Handle
{
public:

	char name[256];
	bool is_directory;
	int index;			//in all_handles array
	int parent_handle_number;
	int size_in_blocks;
	int blocks_numbers[128];
	bool is_existing = false; // существует

	Handle(char name[256] = "", bool is_directory = false)
	{
		strcpy_s(this->name, 256, name);
		this->is_directory = is_directory;
		size_in_blocks = 1;
		parent_handle_number = 0;		//root directory
	}

	
};

