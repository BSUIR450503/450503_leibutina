#include <iostream>
#include <fstream>
#include <conio.h>

using namespace std;

int main()
{
	double input_number = 0;
	double output_number = 0;
	int key = 1;

	fstream input_file;
	input_file.open("exchange", ios::in | ios::binary);
	if (!input_file)
	{
		cout << "Error opening file" << endl;
		_getch();
		return 1;
	}

	input_file >> input_number >> key; // Read data from file
	input_file.clear();
	input_file.sync();
	input_file.close();

	switch (key)
	{
	case 1: output_number = input_number * 2;
		break;
	case 2: output_number = input_number - input_number / 2;
		break;
	case 3: output_number = input_number * input_number;
		break;
	case 4:output_number = input_number / 4;
		break;
	default: cout << "Check the selected operation\n";
		break;
	}

	remove("exchange");

	fstream output_file;
	output_file.open("exchange", ios::app | ios::binary);
	if (!output_file)
	{
		cout << "Error opening file" << endl;
		_getch();
		return 1;
	}
	output_file << output_number;
	output_file.close();

	_getch();
	return 0;
}