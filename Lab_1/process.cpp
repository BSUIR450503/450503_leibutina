#pragma once
#include "libraries.h"


class process 
{
private:
	double input_number;
	double output_number;
	int key;

public:
	process()
	{
		input_number = 0;
		output_number = 0;
		key = 0;
		_getch();
	}

	process(double number, int k)
	{
		input_number = number;
		key = k;
		_getch();
	}

	~process()
	{};

	void show_number()
	{
		cout << "\nInput number: " << input_number << endl;
		cout << "The selected operation is ";
		switch (key)
		{
		case 1: cout << "+\n";
			break;
		case 2: cout << "-\n";
			break;
		case 3: cout << "*\n";
			break;
		case 4: cout << "/\n";
			break;
		default:cout <<"not found\n";
			break;
		}
		cout << "Output number: " << output_number << endl;
	  }

	void start_process()
	{
		fstream input_file;
		input_file.open("exchange", ios::app | ios::binary);  /// проверка 
		input_file << input_number << "\n" << key;
		input_file.close();

#ifdef _WIN32 
		STARTUPINFO StartupInfo;
		PROCESS_INFORMATION ProcInfo;

		TCHAR CommandLine[] = TEXT("pr");
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(StartupInfo);
		ZeroMemory(&ProcInfo, sizeof(ProcInfo));

		if (!CreateProcess(NULL, CommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcInfo))
		{
			cout << "Create Process failed!";
			return;
		}

		WaitForSingleObject(ProcInfo.hProcess, INFINITE);
		CloseHandle(ProcInfo.hProcess);
		CloseHandle(ProcInfo.hThread);

#endif

#ifdef linux
		pid_t pid;
		int status;
		switch (pid = fork()) {
		case -1:
			perror("fork");
			cout << "Error!";
			exit(1);
		case 0:
			execl("processB", "one", "two", "t", NULL);
			exit(0);
		default:
			wait(&status);
		}
#endif

		fstream output_file;
		output_file.open("exchange", ios::in | ios::binary);
		output_file >> output_number;
		output_file.clear();
		output_file.sync();
		output_file.close();
		remove("exchange");
	}
};
