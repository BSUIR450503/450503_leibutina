#include "libraries.h"

int current_ptr_index()
{
    int i;
    for (i = 0; i < MAX_MEMORY_SIZE; i++)
	if (pointers[i].is_busy == 0)
	    return i;
    return -1;
}

int main()
{
    init_memory();
    
    int i;
    char choice = 0;

	while (choice != 'q')
    {	
		if (choice == 'm')
		{
			int size;
			printf("Size = ");
			while (scanf_s("%d", &size) < 1	&& size < 1)
			{
				break;
			}
			while (getchar() != '\n');
		    i = current_ptr_index();
			if ((pointers[i].block = malloc_memory(size)) == NULL )  
			{
				printf("Memory can not be allocated\n");
				continue;
			}
		   pointers[i].is_busy = 1;
		  memset(pointers[i].block->mem_pointer, 'a' + i, size);
		}

		if (choice == 'f')
		{
		 printf("Enter index: ");
		   int index = 0;
			while (scanf_s("%d", &index) < 1 && index < 0)
			while (getchar() != '\n');
		    for (i = 0; i < MAX_MEMORY_SIZE; i++)
			if(pointers[i].is_busy && pointers[i].block->index == index )
			{
			   memset(pointers[i].block->mem_pointer, 0, pointers[i].block->length);
			   free_memory(pointers[i].block->mem_pointer);
			   pointers[i].block = NULL;
			   pointers[i].is_busy = 0;
			   break;
			}				
		}

		if (choice == 'd')
		  defragmentation();

		if (choice == 'r')
		{
			printf("Erner index: ");
			int index = 0;
			int size = 0;
			while (scanf_s("%d", &index) < 1 && index < 0)
			while (getchar() != '\n');
		    printf("New size = ");
	  
			while (scanf_s("%d", &size) < 1 && index < 1)
			while (getchar() != '\n');

			for (int i = 0; i < MAX_MEMORY_SIZE; i++)
			if (pointers[i].is_busy && pointers[i].block->index == i)
			{
				pointers[i].block = realloc_memory(pointers[i].block->mem_pointer,size);
			    break;
			}
		}

		view_memory();
		state();

		printf("\nm  - malloc\nf - free\nr - realloc\nd - defrag\nq - quit\n");
		choice = getchar();
	}
    free_all_memory();
    return 0;
}