#include "frame.h"

uint64_t frames_allocated;

uint64_t frames_available;

#define BITMAP_SIZE 128

// You should define a bitmap to keep track of the allocated frames here.
//extern char bitmap[BITMAP_SIZE];
// 4MB of memory has 1K pages of 4K each.
// A bitmap therefore has 1K entries (one per page), and each byte will take care of 8 entries.
// Therfore, your bitmap should have 128 bytes.

char bitmap[BITMAP_SIZE];

void frame_init() {
	// TODO: Initialize the bitmap
	for (int i = 0; i< BITMAP_SIZE; i++){
		bitmap[i] = (char) 0;
	}
	frames_allocated = 0;
	frames_available = 1024;
}

uint64_t allocate_frame(int number_frames) {
	// TODO: Consult the bitmap and return the first available frame number, marking it as allocated
	int prev = 0;
	int result;
	for (int i = 0; i< BITMAP_SIZE; i++){
		for (int j = 0; j < 8; j++){

			if (!((bitmap[i]>>(8-j-1)) & 1)){
				if(!prev) result = i * 8 + j;
				//bitmap[i] = bitmap[i] | (1<<j);
				prev++;
				if (prev == number_frames) {
					for (int k = 0; k < number_frames; k++){
						int row = result / 8;
						int col = result % 8;
						bitmap[row + (k+col)/8] = bitmap[row + (k+col)/8] | (1<<(8-(col+k)%8-1));
						frames_allocated++;
						frames_available--;
					}
					return result;
				}
			}
			else{
				prev = 0;
			}
		}
	}

	return -1;
}

int deallocate_frame(uint64_t frame_number, int number_frames) {
	// TODO: Mark the frame as deallocated in the bitmap
	int i = frame_number / 8;
	int j = frame_number % 8;
	if (((bitmap[i]>>(8-j-1)) & 1)){
		for (int k = 0; k < number_frames; k++){
			bitmap[i + (j + k) / 8] = bitmap[i + (j + k) / 8] & ~(1 << (8-(j+k)%8-1));
			frames_allocated--;
			frames_available++;
		}
		//bitmap[i] = bitmap[i] & !(1 << j);
		return frame_number;
	}


	return -1;
}
