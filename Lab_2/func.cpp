#include "librariesh.h"

class process
{
private:
	string value[amount];
public:
	process()
	{
		value[0] = "1.First process\n";
		value[1] = "2.Second process\n";
		value[2] = "3.Third process\n";
		value[3] = "4.Fourth process\n";
		value[4] = "5.Fifth process\n";
		value[5] = "6.Sixth process\n";
		value[6] = "7.Seventh process\n";
		value[7] = "8.Eighth process\n";
		value[8] = "9.Ninth process\n";
		value[9] = "10.Tenth process\n";
	}

	~process()
	{ }

#ifdef _WIN32
	int catch_input()
	{
		if (_kbhit())
			return _getch();
		else
			return -1;
	}
#endif

#ifdef linux 
	int flag_to_print = 0;
	int flag_to_end = 1;

	struct sigaction Signal_to_print;
	struct sigaction Signal_to_end;

	void set_print(int sig)
	{
		flag_to_print = 1;
	}

	void set_end(int sig)
	{
		flag_to_end = 1;
	}
#endif

	void printString(int processNumber)
	{
#ifdef _WIN32	
		char EventID[30];
		sprintf_s(EventID, " %d", processNumber);
		HANDLE closeEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EventID);

		sprintf_s(EventID, " %dp", processNumber);
		HANDLE CanPrintEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EventID);

		while (1)
		{
			if (WaitForSingleObject(CanPrintEvent, 1) == WAIT_OBJECT_0)
			{
				for (int i = 0; i<value[processNumber - 1].length(); i++)     
				{
					if (WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0)  // Если установлено событие закрытия данного процесса	
					{
						CloseHandle(closeEvent);
						CloseHandle(CanPrintEvent);
						return;
					}
					cout << value[processNumber - 1][i];
					Sleep(50);
				}
				ResetEvent(CanPrintEvent);
			}
			if (WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0)
			{
				CloseHandle(closeEvent); CloseHandle(CanPrintEvent);
				return;
			}
		}
#endif

#ifdef linux 
		for (int i = 0; i < value[processNumber - 1].length(); i++)
		{
			cout << value[processNumber - 1][i];
			fflush(stdout);
			Sleep(5000);
		}
#endif 
		return;
	}

	void Work_Process(char *path)
	{
		char choice = 0;  
		int currentNum = 0; 
		bool flag = false; 
		PROCESS_INFORMATION procInf[amount];  
		int number_of_process = 0;
#ifdef _WIN32
		vector<HANDLE> Events;
		vector<HANDLE> CanPrintEvents;

		char EventID[30];
#endif

#ifdef linux 
		
		Signal_to_print.sa_handler = set_print;
		sigaction(SIGUSR1, &Signal_to_print, NULL);

		Signal_to_end.sa_handler = set_end;
		sigaction(SIGUSR2, &Signal_to_end, NULL);
#endif 
		while (1)
		{
			choice = catch_input();
			Sleep(50);
			switch (choice)
			{
			case '+':
			{
#ifdef _WIN32
						if (Events.size() < amount)
						{
							sprintf(EventID, " %d", Events.size() + 1); // Number to String 
							Events.push_back(CreateEvent(NULL, FALSE, FALSE, EventID)); 

							sprintf_s(EventID, " %dp", CanPrintEvents.size() + 1);  
							CanPrintEvents.push_back(CreateEvent(NULL, TRUE, FALSE, EventID)); 

							procInf[Events.size() - 1] = CreateNewProcess(path, EventID);
						}
#endif 

#ifdef linux 
						if (number_of_process < amount - 1)
						{
							procInf[number_of_process] = fork();
							number_of_process++;

							switch (procInf[number_of_process - 1])
							{
							case 0:
							{
									  flag_to_end = 0;
									  while (!flag_to_end)
									  {
										  Sleep(SLEEP_TIME_ONE);
										  if (flag_to_print)
										  {
											  printString(number_of_process); 
											  flag_to_print = 0;
											  kill(getppid(), SIGUSR2);
										  }
									  }
									  break;
							}
							case -1:
							{
									   cout << "CreateProcess failed!" << endl;
									   break;
							}
							default:
							{
									   break;
							}
							}
						}
#endif 
						break;
			}
				////////////////////////////////////////////////////////////////////
			case '-':
			{
#ifdef _WIN32
						if (Events.size() > 0)
						{
							SetEvent(Events.back());
							WaitForSingleObject(procInf[Events.size() - 1].hProcess, INFINITE); // Ожидаем завершения последнего процесса

							if (!CloseHandle(Events.back()))
								printf("Close Handle failed (%d)\n", GetLastError());
							if (!CloseHandle(CanPrintEvents.back()))
								printf("Close Handle failed (%d)\n", GetLastError());

							Events.pop_back();  // удалили последнее событие 
							CanPrintEvents.pop_back();
							if (currentNum >= Events.size())
							{
								currentNum = 0;
								flag = true;
							}
						}
#endif 

#ifdef linux 
						if (number_of_process > 0)
						{
							kill(procInf[number_of_process - 1], SIGUSR2);
							number_of_process--;
						}
#endif 
						break;
			}
			case 'q':
			{
#ifdef _WIN32			
						if (Events.size() > 0)
						{
							while (Events.size() > 0)
							{
								SetEvent(Events.back());
								WaitForSingleObject(procInf[Events.size() - 1].hProcess, INFINITE); // Ожидаем завершения последнего процесса

								if (!CloseHandle(Events.back()) || !CloseHandle(CanPrintEvents.back()) ||
									!CloseHandle(procInf[Events.size() - 1].hProcess)
									|| !CloseHandle(procInf[Events.size() - 1].hThread))
									printf("Close Handle failed (%d)\n", GetLastError());

								Events.pop_back();
								CanPrintEvents.pop_back();
							}
							currentNum = 0;
						}
						printf("\n\n");
						return;
#endif 
#ifdef linux 
						if (number_of_process > 0)
						{
							for (; number_of_process >= 0; number_of_process--)
							{
								kill(procInf[number_of_process - 1], SIGUSR2);
								waitpid(procInf[number_of_process - 1], NULL, 0);
							}
						}
						return;
#endif 
			}
			}

#ifdef _WIN32
			if (CanPrintEvents.size() > 0 && WaitForSingleObject(CanPrintEvents[currentNum], 0) == WAIT_TIMEOUT) 
			{
				if (currentNum >= CanPrintEvents.size() - 1)
					currentNum = 0;
				else if (!flag) currentNum++;
				flag = false;
				SetEvent(CanPrintEvents[currentNum]);
			}
#endif 

#ifdef linux 
			if (flag_to_end && number_of_process > 0)
			{
				flag_to_end = 0;
				if (currentNum >= number_of_process - 1)
				{
					currentNum = 0;
				}
				else
				{
					if (!flag)
					{
						currentNum++;
					}
				}
				flag = 0;
				kill(procInf[currentNum], SIGUSR1);
			}
#endif 
		}
	}

#ifdef _WIN32 
	PROCESS_INFORMATION CreateNewProcess(char* path, char *commandline)
	{
		STARTUPINFO StartupInfo;
		PROCESS_INFORMATION infProc;

		TCHAR CommandLine[] = TEXT("pr");
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(StartupInfo);
		ZeroMemory(&infProc, sizeof(infProc));

		if (!CreateProcess(path, commandline, NULL, NULL, FALSE, NULL, NULL, NULL, &StartupInfo, &infProc))
		{
			cout << "Create Process failed!";
		}
		return infProc;
	}
#endif
};


