#include "libraries.h"
#include "process.cpp"

int main() 
{ 
	double number;
	int k;
	cout << "Enter number : ";
	cin >> number;
	
	cin.clear();
	cin.sync();
	 
	cout << "\nEnter the key of necessary operation\n";
	cout << "1 - '+'\n2 - '-'\n3 - '*'\n4 - '/'\n";
	cin >> k;
	cin.clear();
	cin.sync();

	process numberclass(number, k);
	numberclass.start_process();
	numberclass.show_number();

	_getch();
	return 0;
}
