#include "kmalloc.h"

// After you are done, you don't need to import it anymore. This is here just for the dummy code on malloc/free.
#include<stdlib.h>

uint64_t * start = NULL;
uint64_t MAGIC = 0xabcdef;

void *palloc(uint64_t number) {
	// TODO:
	// 1) Allocate 1 frame of memory
	// 2) Find the first page number (virtual) that is not mapped to a frame (physical)
	// 3) Map the found page number (virtual) to the allocated frame address (not the frame number, but the frame address)
	// 4) Return the virtual address of the first byte of the allocated frame

	uint64_t frame = allocate_frame(number);
	if (frame == -1) return -1;
	uint64_t page = vm_locate(number);
	int sucess = vm_map(page, frame, number, 0);
	if (!sucess) return -1;
	return memory + 4096 * frame;

	// Dummy code: you cannot use malloc/free
	//return malloc(4196 * number);

}

void pfree(void* address, uint64_t number) {
	// TODO:
	// 1) Unmap the page number starting at the provided address (the address is the first byte of a page)
	// 2) Do that for the number of times provided in the parameter number

	//uint64_t page = GET_PAGE_NUMBER(address)
	//uint64_t frame = vm_translate(address);
	//
	//vm_unmap(page);
	uint64_t frame = ((uint64_t)address - (uint64_t)memory) / 4096;
	deallocate_frame(frame, number);

	// Dummy code: you cannot use malloc/free
	//free(address);
}

void *kmalloc(uint64_t size) {
	// TODO:
	// - Implement a linked list of free chunks using only palloc()
	if (!start){ //create a new linked list
		uint64_t number = (size/4096 + 1);
		start = palloc(number);
		//assigning used chunk size and magic number
		*start = size + 2* sizeof(uint64_t);
		*(start + 1) = MAGIC;
		start += 2;
		//main data content of the assigned chunk;
		char * content = (char *)(start);
		//start redirected to after the main data chunk
		start = (uint64_t)(content + size);
		//remaining memory in the chunk
		*start = (number * 4096 - size - 2 * sizeof(uint64_t));
		*(start+1) = 0;
		return (uint64_t*) content;
	}
	else { //The linked list is already created, so find the next avaliable chunk
		//with enough space
		uint64_t * temp = start;
		uint64_t * prev = NULL;
		while (*(temp+1) && ((size + 2 * sizeof(uint64_t)) > *temp)){
			//The first free chunk with enough space, or the last chunk of the page
			prev = temp;
			temp = * (temp + 1);
		}
		if ((size + 2 * sizeof(uint64_t)) <= *temp){
			//The chunk is large enough
			uint64_t * next_free = *(temp + 1);
			uint64_t current_size = *temp;
			*temp = size + 2 * sizeof(uint64_t);
			*(temp + 1) = MAGIC;
			temp += 2;
			//main data content of the assigned chunk;
			char * content = (char *)(start);
			//temp redirected to after the main data chunk
			temp = (uint64_t)(content + size);
			*temp = (current_size - size - 2 * sizeof(uint64_t));
			if (prev) *(prev + 1) = temp;
			*(temp + 1) = next_free;
			return (uint64_t*) content;
		}
		else{
			//allocate another chunk
			uint64_t number = (size/4096 + 1);
			uint64_t * new_start = palloc(number);
			//assigning used chunk size and magic number
			*new_start = size + 2* sizeof(uint64_t);
			*(new_start + 1) = MAGIC;
			new_start += 2;
			//main data content of the assigned chunk;
			char * content = (char *)(new_start);
			//start redirected to after the main data chunk
			new_start = (uint64_t)(content + size);
			//remaining memory in the chunk
			*new_start = (number * 4096 - size - 2 * sizeof(uint64_t));
			*(new_start+1) = 0;
			*(temp + 1) = new_start;
			return (uint64_t*) content;
		}
	}
	// - Use the first-fit strategy to allocate a chunk
	// Dummy code: you cannot use malloc/free
	//return malloc(size);
}

void *krealloc(void *address, uint64_t size) {
	// TODO:
	//
	uint64_t * first = address - 2;
	uint64_t former_size = *first;
	uint64_t * next = (uint64_t*)((char*)address + former_size - 2 * sizeof(uint64_t));
	// - If the address is becoming smaller, return the last frames that have become unused with vm_unmap() and frame_deallocate()
	if (former_size > size){
		uint64_t * new_start = (uint64_t)((char*) address + size);
		*new_start = former_size - size;
		*(new_start + 1) = *start;
		start = new_start;
		if (*(next+1) != MAGIC){
			//TODO:
			//if next chunk is free merge the two free parts
			merge_free(new_start, next);
		}
		else{
			//next chunk is USED
			*first = size;
		}
		return address;
	}
	// - If the address is becoming bigger, and is possible to allocate new contiguous pages to extend the chunk size,
	else{
		//check if the next block is free and big enough
		if (*(next+1)!=MAGIC && (*(next)+first) > size){
			//the next block is free and big enough
			*first = size;
			//check if there is space left after reallocation
			if (former_size + *next > size){
				//TODO:
				//create new free chunk
				uint64_t * new_start = (uint64_t)((char*) address + size);
				*new_start = former_size + *(next) - size;
				*(new_start+1) = *(next+1);
				//make prev point to new_start
				uint64_t * prev = prev_finder(next);
				*(prev+1) = new_start;
			}
			else{
				//No space left after reallocation
				if (*(next+1)){
					//this chunk being used for expansion is not the last of free chunks
					//change what used to point to next to *(next+1)
					uint64_t * prev = prev_finder(next);
					*(prev + 1) = *(next + 1);
				}
				else{
					//No free chunks left after this chunk is used
					uint64_t * prev = prev_finder(next);
					*(prev+1) = 0;
				}

			}

		}
		else{
			//need to allocate new 4k chunk
			uint64_t * new_start = kmalloc(size);
			*(new_start - 1) = MAGIC;
			*(new_start - 2) = size;
			memcpy(new_start, address, former_size);
			kfree(address);
			return new_start;
		}

	}
	//   allocate new frames and map their pages.
	// - If the address is becomming bigger, but it is not possible to allocate new contigous pages to extend the chunk size,
	//   use kmalloc() to allocate a new chunk, then memcpy() to copy the original bytes to the new chunk, and return the new chunk's address.
	//   Before returning, use kfree() to free the old chunk.

	// Dummy code: you cannot use malloc/free... or realloc
	//return realloc(address, size);
}

void kfree(void *address) {
	// TODO:
	// - Make the space used by the address free
	uint64_t * first = address - 2;
	uint64_t former_size = *first;
	uint64_t * next = (uint64_t*)((char*)address + former_size - 2 * sizeof(uint64_t));
	if(*(next+1) == MAGIC){
		//Next chunk is used, set this freed chunk to the start of the list.
		*(first + 1) = *start;
		start = first;
	}
	else{
		//Next chunk is free
		merge_free(first,next);
	}


	// - Return any frames that have become unused with vm_unmap() and frame_deallocate()

	// Dummy code: you cannot use malloc/free
	//return free(address);
}

uint64_t * prev_finder(uint64_t * current){
	uint64_t * temp = start;
	while (*(temp+1) && *(temp+1) != current){
		temp = *(temp + 1);
	}
	return temp;
}

void merge_free(uint64_t * start, uint64_t * end){
	uint64_t * next = *(end + 1);
	*start += *end;
	*(start + 1) = next;
}
