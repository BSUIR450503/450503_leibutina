#pragma once

class Directory
{
public:
	static const int MAX_HANDLES_COUNT = 256;
	int handles_count;
	int handle_numbers[MAX_HANDLES_COUNT];

	Directory()
	{
		handles_count = 0;
	}
};

