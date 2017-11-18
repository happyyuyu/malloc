#include "kmalloc.h"

// After you are done, you don't need to import it anymore. This is here just for the dummy code on malloc/free.
#include<stdlib.h>

void *palloc(uint64_t number) {
	// TODO:
	// 1) Allocate 1 frame of memory
	// 2) Find the first page number (virtual) that is not mapped to a frame (physical)
	// 3) Map the found page number (virtual) to the allocated frame address (not the frame number, but the frame address)
	// 4) Return the virtual address of the first byte of the allocated frame

	uint64_t frame = allocate_frame(...);
	if (frame == -1) return -1;
	uint64_t page = vm_locate(...)
	vm_map(page, frame, /* one page */, 0);
	return GET_FIRST_BYTE(frame);

	// Dummy code: you cannot use malloc/free
	//return malloc(4196 * number);

}

void pfree(void* address, uint64_t number) {
	// TODO:
	// 1) Unmap the page number starting at the provided address (the address is the first byte of a page)
	// 2) Do that for the number of times provided in the parameter number

	uint64_t page = GET_PAGE_NUMBER(address)
	uint64_t frame = vm_translate(page)
	//
	vm_unmap(page);
	deallocate_frame(frame);

	// Dummy code: you cannot use malloc/free
	//free(address);
}

void *kmalloc(uint64_t size) {
	// TODO:
	// - Implement a linked list of free chunks using only palloc()
	// - Use the first-fit strategy to allocate a chunk



	// Dummy code: you cannot use malloc/free
	return malloc(size);
}

void *krealloc(void *address, uint64_t size) {
	// TODO:
	//
	// - If the address is becoming smaller, return the last frames that have become unused with vm_unmap() and frame_deallocate()
	// - If the address is becoming bigger, and is possible to allocate new contiguous pages to extend the chunk size,
	//   allocate new frames and map their pages.
	// - If the address is becomming bigger, but it is not possible to allocate new contigous pages to extend the chunk size,
	//   use kmallloc() to allocate a new chunk, then memcpy() to copy the original bytes to the new chunk, and return the new chunk's address.
	//   Before returning, use kfree() to free the old chunk.

	// Dummy code: you cannot use malloc/free... or realloc
	return realloc(address, size);
}

void kfree(void *address) {
	// TODO:
	// - Make the space used by the address free
	// - Return any frames that have become unused with vm_unmap() and frame_deallocate()

	// Dummy code: you cannot use malloc/free
	return free(address);
}
