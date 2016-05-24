#include "libraries.h"

uint8_t memory[MAX_MEMORY_SIZE];
memory_pointer pointers[MAX_MEMORY_SIZE];
mblock_t* table= NULL;

mblock_t* get_block(void* pointer)
{
    mblock_t* iter = table;
    while (iter)
	if (iter->mem_pointer == pointer)
	    return iter;
        else
	    iter = iter->next;
    return NULL;
}

void defragmentation()
{
    mblock_t* iter = table;
    if (iter == NULL)
	return;
    
    while (iter->next)
    {
	if (iter->is_free && iter->next->is_free)
	{
	    mblock_t* second = iter->next;

	    iter->length += second->length;
	    iter->next = second->next;
	    if (second->next != NULL)
		second->next->prev = iter;

		free(second);	    
	    continue;
	}
	
	if (iter->is_free && !(iter->next->is_free) )
	{
	    uint8_t i;
	    for (i = 0; i < iter->next->length;i++)
	    {
			memory[ iter->index + i ] = iter->next->mem_pointer[i];
	    }
	    i += iter->index;
	    for ( ; i < (iter->next->index + iter->next->length); i++)
			memory[i] = 0;
	    iter->is_free = 0;
	    iter->next->is_free = 1;

	    for (i = 0; i < MAX_MEMORY_SIZE; i++)
			if (pointers[i].block == iter->next)
			{
			    pointers[i].block = iter;
			    break;
			}

	    i = iter->length;
	    iter->length = iter->next->length;
	    iter->next->length = i;
	    iter->next->index = iter->index + iter->length;    	    
	    iter->mem_pointer = memory + iter->index;
	}
	iter = iter->next;
    }
}

void init_memory()
{
    table= (mblock_t*)malloc(sizeof(mblock_t));
    table->is_free = 1;
    table->index = 0;
    table->length = MAX_MEMORY_SIZE;
    table->next = NULL;
    table->prev = NULL;
    table->mem_pointer = NULL;
}

mblock_t* malloc_memory(uint8_t size)
{
    mblock_t* iter = table;

    while(iter)
    {
		if(iter->is_free)
		{
		    if(iter->length > size)
		    {
				mblock_t* new_block = (mblock_t *)malloc(sizeof(mblock_t));
				new_block->is_free = 0;
				new_block->length = size;
				new_block->index = iter->index;
				new_block->next = iter;
				new_block->prev = iter->prev;
				if (new_block->prev == NULL)
				    table= new_block;
				else
				    new_block->prev->next = new_block;
				new_block->mem_pointer = memory + iter->index;
				iter->prev = new_block;
				iter->index += size;  
				iter->length -= size;	    
				return new_block;
		    }

		    if(iter->length == size)
		    {
				iter->is_free = 0;
				iter->mem_pointer = memory + iter->index;
				return iter;
		    }
		}
		iter = iter->next;
		}
	return NULL;
}

void free_memory(void* fr_pointer)
{
    mblock_t * block_for_del = get_block(fr_pointer);
    if (block_for_del != NULL)
    {
		block_for_del->is_free = 1;
		block_for_del->mem_pointer = NULL;
		for (uint8_t i = block_for_del->index; i < block_for_del->length; i++)
		{
			memory[i] = 0;
		}
    } 
}

mblock_t* realloc_memory(void* ptr, uint8_t size)
{
    mblock_t* new_ptr_blk = malloc_memory(size);
    if (new_ptr_blk == NULL)
		return get_block(ptr);
    memcpy(new_ptr_blk->mem_pointer,  ptr,get_block(ptr)->length);
    free_memory(ptr);
    return new_ptr_blk;
}

void free_all_memory()
{
    mblock_t* rem;
    while (table)
    {
		rem = table;
		table= table->next;
		free(rem);
    }
}

void state()
{
    mblock_t* output = table;	

    printf("\nType\tFirst\tLast\tSize\n");
	
	while (output)
    {       
	printf("%s\t %d\t %d\t %d\n",
		output->is_free ? "Free" : "Busy",
		output->index,
		output->index + output -> length - 1,
		output->length);
		output = output->next;
    }
}

void view_memory()
{
    for (uint8_t i = 0; i < MAX_MEMORY_SIZE; i++) 
	printf("%c", memory[i] ? memory[i] : '.');
    printf("\n");
}