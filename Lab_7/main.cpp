#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <fstream>
#include "Handle.h"
#include "Directory.h"

using namespace std;



#define BLOCK_SIZE 64
#define HANDLES_COUNT 32
#define BLOCKS_COUNT 320


Handle *createArea(char name[256], bool is_directory, bool is_add_existing = false);

const char *path = "D:\\FileSystem.txt";

const int offset_all_handles = 0; 
const int offset_busy_handles = offset_all_handles + HANDLES_COUNT * sizeof Handle;
const int offset_busy_blocks = offset_busy_handles + HANDLES_COUNT;
const int offset_data = offset_busy_blocks + BLOCKS_COUNT;

Handle all_handles[HANDLES_COUNT];			
bool busy_handles[HANDLES_COUNT];			//true 
bool busy_blocks[BLOCKS_COUNT];				//true 

Handle *current_dir;
int current_dir_content_size;
Handle *current_dir_content[256];
char addFileName[256];

template <class T>
void read(T *param, int offset = 0, int count = 1, const char* mainPath = path) {
	ifstream in(mainPath, ios::binary | ios::in);
	in.seekg(offset);
	in.read((char*)param, sizeof(T)*count);
	in.close();
}

template <class T>
void write(T *param, int offset, int count = 1, const char* mainPath = path) {
	ofstream out(mainPath, ios::binary | ios::in | ios::out);
	out.seekp(offset);
	out.write((char*)param, sizeof(T)*count);
	out.close();
}

void createFS(){
	for (int i = 0; i < BLOCKS_COUNT; i++)
		busy_blocks[i] = false;
	for (int i = 0; i < HANDLES_COUNT; i++)
		busy_handles[i] = false;

	write(busy_handles, offset_busy_handles, HANDLES_COUNT);
	write(busy_blocks, offset_busy_blocks, BLOCKS_COUNT);

	current_dir = createArea("root", true);
}

void loadFS() {
	ifstream in(path, ios::binary | ios::in);

	read(all_handles, offset_all_handles, HANDLES_COUNT);
	read(busy_handles, offset_busy_handles, HANDLES_COUNT);
	read(busy_blocks, offset_busy_blocks, BLOCKS_COUNT);

	current_dir = &all_handles[0];
}

int takeHandle(){
	for (int i = 0; i < HANDLES_COUNT; i++)
	if (!busy_handles[i]){
		busy_handles[i] = true;
		return i;
	}
	return -1;
}

int takeBlock() {
	for (int i = 0; i < BLOCKS_COUNT; i++)
	if (!busy_blocks[i]) {
		busy_blocks[i] = true;
		return i;
	}
	return -1;
}

void freeHandle(int handleNumber) {
	busy_handles[handleNumber] = false;
}

void freeBlock(int blockNumber) {
	busy_blocks[blockNumber] = false;
}

Handle *createArea(char name[256], bool is_directory = false, bool is_add_existing) {
	long int file_size;
	Handle handle(name, is_directory);
	int block_index = takeBlock(); 
	handle.blocks_numbers[0] = block_index;

	int handle_index = takeHandle();
	handle.index = handle_index;		//each handle remember it's position in array
	if (is_add_existing){
		handle.is_existing = true;
	}
	all_handles[handle_index] = handle;  

	write(&handle, offset_all_handles + handle_index*sizeof Handle);
	write(&busy_handles[handle_index], offset_busy_handles + handle_index);
	write(&busy_blocks[block_index], offset_busy_blocks + block_index);

	if (is_directory) {
		Directory dir;
		write(&dir, offset_data + block_index*BLOCK_SIZE);
	}
	else {
		if (is_add_existing) {
			ifstream in(name, ios::binary | ios::in);
			in.seekg(0, ios::end);
			file_size = in.tellg(); 
			printf("SOME  %d", file_size);
			write(&file_size, offset_data + block_index*BLOCK_SIZE);
		}
		else {
			file_size = 0;
			write(&file_size, offset_data + block_index*BLOCK_SIZE);
		}

	}

	return &all_handles[handle_index];
}

Handle *getParent(Handle *handle) {
	return &all_handles[handle->parent_handle_number];
}

void addToDir(Handle *hDir, Handle *handle) {
	int block = hDir->blocks_numbers[0];
	Directory dir;
	read(&dir, offset_data + block*BLOCK_SIZE);
	if (dir.handles_count == dir.MAX_HANDLES_COUNT)
		return;
	handle->parent_handle_number = hDir->index;
	dir.handle_numbers[dir.handles_count++] = handle->index;
	write(&dir, offset_data + block*BLOCK_SIZE);
	write(handle, offset_all_handles + handle->index *sizeof Handle);
}

string getPath(Handle *handle = current_dir, string path = "") {
	string s1 = handle->name;
	string s2 = (handle->is_directory) ? "\\" : "";
	path = s1 + s2 + path;
	if (handle->index > 0)
		return getPath(getParent(handle), path);
	return path;
}

void printOptions(bool is_directory = true) {
	if (is_directory)
		cout << "0 - main , 1 - open, 2 - rename, 3 - delete, 4 -create, 5 - exit\n";
	else
		cout << "~ - save and exit\n";
	cout << "________________________________________________________________________________\n";
}


void printHeader(Handle *handle) {
	system("CLS");
	cout << getPath(handle) << "\n________________________________________________________________________________\n";
	printOptions(handle->is_directory);
}

void openDir(Handle *hDir) {
	current_dir = hDir;
	int block = hDir->blocks_numbers[0];
	Directory dir;
	read(&dir, offset_data + block*BLOCK_SIZE);

	printHeader(hDir);
	current_dir_content_size = dir.handles_count;
	for (int i = 0; i < dir.handles_count; i++)
	{
		Handle *current = &all_handles[dir.handle_numbers[i]];
		current_dir_content[i] = current;
		printf("%3d | ", i + 1);
		cout << ((current->is_directory) ? "[Dir.] " : "[File] ");
		cout << current->name << endl;
	}
}

int getBlockOffset(int blockNumber) {
	return offset_data + blockNumber*BLOCK_SIZE;
}

void readWriteFile(Handle *hFile, char* buffer, long int file_size, bool writing, bool is_add_existing = false) {
	int current_block = hFile->blocks_numbers[0];		//block number in memory
	long int current_offset = getBlockOffset(current_block);
	current_offset += sizeof file_size;
	long int not_readed_bytes = file_size;
	long int readed_bytes = 0;
	long int bytes_to_read = (file_size > BLOCK_SIZE - sizeof file_size) ? BLOCK_SIZE - sizeof file_size : file_size;

	int i = 0;			//block number in file
	while (not_readed_bytes > 0)
	{
		if (writing)
		if (is_add_existing){
			write(buffer + readed_bytes, 0, bytes_to_read, addFileName);
			write(buffer + readed_bytes, current_offset, bytes_to_read);
		}
		else {
			write(buffer + readed_bytes, current_offset, bytes_to_read);
		}
		else {
			if (is_add_existing){
				read(buffer + readed_bytes, 0, bytes_to_read, addFileName);
				write(buffer + readed_bytes, current_offset, bytes_to_read);
			}
			else {
				read(buffer + readed_bytes, current_offset, bytes_to_read);
			}
		}
		readed_bytes += bytes_to_read;
		not_readed_bytes -= bytes_to_read;
		bytes_to_read = (not_readed_bytes > BLOCK_SIZE) ? BLOCK_SIZE : not_readed_bytes;
		current_block = hFile->blocks_numbers[++i];
		current_offset = getBlockOffset(current_block);
	}
}

void saveFile(Handle *hFile, char* buffer, long int file_size) {
	while (file_size > hFile->size_in_blocks*BLOCK_SIZE - sizeof file_size)
		hFile->blocks_numbers[hFile->size_in_blocks++] = takeBlock();
	while ((hFile->size_in_blocks - 1)*BLOCK_SIZE >= file_size)
		freeBlock(hFile->blocks_numbers[--hFile->size_in_blocks]);

	int current_block = hFile->blocks_numbers[0];		
	long int current_offset = getBlockOffset(current_block);
	write(&file_size, current_offset);
	if (hFile->is_existing){
		readWriteFile(hFile, buffer, file_size, true, true);
	}
	else{
		readWriteFile(hFile, buffer, file_size, true);
	}
}

void editFile(Handle *hFile, bool is_add_existing = false) {
	int block_index = hFile->blocks_numbers[0];
	long int file_size;
	read(&file_size, offset_data + block_index*BLOCK_SIZE);
	char* temp = new char[file_size + 1];
	if (is_add_existing){
		readWriteFile(hFile, temp, file_size, false, true);
	}
	else {
		readWriteFile(hFile, temp, file_size, false);
	}
	temp[file_size] = '\0';

	printHeader(hFile);
	cout << temp;

	vector<char> buffer;
	for (int i = 0; i < file_size; i++)
		buffer.push_back(temp[i]);
	char c;
	while ((c = _getch()) != '~')
	{
		switch (c)
		{
		case '\r':
			buffer.push_back('\n');
			putchar('\n');
		default:
			buffer.push_back(c);
			putchar(c);
			break;
		}
	}

	temp = new char[buffer.size()];
	for (int i = 0; i < buffer.size(); i++)
		temp[i] = buffer[i];

	saveFile(hFile, temp, buffer.size());
	openDir(getParent(hFile));
}

Directory getDirByHandle(Handle *handle) {
	int block = handle->blocks_numbers[0];
	Directory dir;
	read(&dir, offset_data + block*BLOCK_SIZE);
	return dir;
}

void rename(Handle *handle, char newName[256]) {
	strcpy_s(handle->name, newName);
	write(handle, offset_all_handles + handle->index *sizeof Handle);
}

void remove(Handle *handle) {
	Handle *hParentDir = &all_handles[handle->parent_handle_number];
	Directory pDir = getDirByHandle(hParentDir);
	int i = 0;
	while (pDir.handle_numbers[i] != handle->index && i < pDir.handles_count)
		i++;
	for (; i < pDir.handles_count - 1; i++)
		pDir.handle_numbers[i] = pDir.handle_numbers[i + 1];
	pDir.handles_count--;

	int block = hParentDir->blocks_numbers[0];
	write(&pDir, offset_data + block*BLOCK_SIZE);

	freeHandle(handle->index);
}

void loadOption(int n) {
	//0 - go up, 1 - open, 2 - rename, 3 - delete, 4 -create\n
	Directory dir = getDirByHandle(current_dir);
	int number;
	char newName[256];
	switch (n) {
	case 0:
		openDir(&all_handles[current_dir->parent_handle_number]);
		break;
	case 1:
		cout << "Number ";
		number = 0;
		while (number<1 || number>current_dir_content_size)
			cin >> number;
		number--;
		if ((dir.handles_count > 0) && number >= 0 && number < dir.handles_count)
		{
			Handle *handle = &all_handles[dir.handle_numbers[number]];
			if (handle->is_directory)
				openDir(handle);
			else
			if (handle->is_existing){
				editFile(handle, true);
			}
			else{
				editFile(handle);
			}
		}
		break;
	case 2:
		cout << "Number ";
		number = 0;
		while (number<1 || number>current_dir_content_size)
			cin >> number;
		cout << "New name: ";
		cin >> newName;
		rename(current_dir_content[number - 1], newName);
		openDir(current_dir);
		break;
	case 3:
		cout << "Number ";
		number = 0;
		while (number<1 || number>current_dir_content_size)
			cin >> number;
		remove(current_dir_content[number - 1]);
		openDir(current_dir);
		break;
	case 4:
		cout << "0 - dir, X - file\n";
		cin >> number;
		cout << "Name: ";
		cin >> newName;
		addToDir(current_dir, createArea(newName, (number == 0)));
		openDir(current_dir);
		break;
	/*case 5:
		cout << "Type the path to the file: " << endl;
		cin >> addFileName;
		addToDir(current_dir, createArea(addFileName, false, true));
		openDir(current_dir);
		break;
		*/
	}
}

int main() {
//createFS();
	 loadFS();
	openDir(current_dir);
	int selector;
	while (1) {
		fflush(stdin);
		cin >> selector;
		if (selector == 5){
			break;
		}
		loadOption(selector);
	}
	return 0;
}