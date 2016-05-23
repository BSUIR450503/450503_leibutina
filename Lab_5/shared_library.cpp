#include "libraries.h"
extern "C" 
{
	struct Information 
	{
#ifdef _WIN32
		HANDLE hFile; 
		DWORD number_of_bytes;
		CHAR buffer[SIZE];
		DWORD input_position;
		DWORD output_position;
		OVERLAPPED Overlapped;
#elif __linux__
		int hFile;
		char buffer[SIZE];
		size_t number_of_bytes;
		size_t number_of_processed_bytes;
		off_t input_position;
		off_t output_position;
		struct aiocb aiocbStruct;
#endif
	};

#ifdef _WIN32
	__declspec(dllexport) BOOL read(Information *information) { 
		BOOL info_from_file;
		DWORD number_of_processed_bytes;
		information->Overlapped.Offset = information->input_position; 

		ReadFile(information->hFile, information->buffer, information->number_of_bytes, NULL, &information->Overlapped);
		WaitForSingleObject(information->Overlapped.hEvent, INFINITE);  
		info_from_file = GetOverlappedResult(information->hFile, &information->Overlapped, &number_of_processed_bytes, FALSE); 
		if (info_from_file) {
			information->input_position = information->input_position + number_of_processed_bytes;
		}
		return info_from_file;
#elif __linux__
	int read_async(struct Information *information) {
		information->aiocbStruct.aio_offset = information->input_position;
		information->aiocbStruct.aio_fildes = information->hFile;
		information->aiocbStruct.aio_nbytes = information->number_of_bytes;
		aio_read(&information->aiocbStruct);

		while (aio_error(&information->aiocbStruct) == EINPROGRESS); 
		information->number_of_processed_bytes = aio_return(&information->aiocbStruct); 
		if (information->number_of_processed_bytes) {
			information->input_position = information->input_position + information->number_of_processed_bytes;
		}
		return information->number_of_processed_bytes;
#endif
	}

#ifdef _WIN32
	__declspec(dllexport) BOOL write(Information *information) { 
		BOOL info_for_file;
		DWORD number_of_processed_bytes;
		information->Overlapped.Offset = information->output_position; 

		WriteFile(information->hFile, information->buffer, information->Overlapped.InternalHigh, NULL, &information->Overlapped);
		WaitForSingleObject(information->Overlapped.hEvent, INFINITE);
		info_for_file = GetOverlappedResult(information->hFile, &information->Overlapped, &number_of_processed_bytes, FALSE); 
		
		if (info_for_file) {
			information->output_position = information->output_position + number_of_processed_bytes;
		}
		return info_for_file;
#elif __linux__
	int write_async(struct Information *information) {
		int info_for_file;
		information->aiocbStruct.aio_offset = information->output_position;
		information->aiocbStruct.aio_fildes = information->hFile;
		information->aiocbStruct.aio_nbytes = information->number_of_processed_bytes;
		aio_write(&information->aiocbStruct);
		while ((info_for_file = aio_error(&information->aiocbStruct)) == EINPROGRESS);
		information->output_position = information->output_position + aio_return(&information->aiocbStruct);
		return info_for_file;
#endif
	}
}