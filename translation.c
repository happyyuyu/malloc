#include<stdio.h>

#include "translation.h"
#include<frame.h>;

struct entry_t_ *table_root = NULL;

int vm_map(uint64_t page, uint64_t frame, int number, uint16_t flags) {
	struct entry_t_ *temp_root = table_root;
	uint64_t indices[4]; /// page: ___0__(28)___(9)___(9)___(9)___(9)
	uint64_t indices[0] = (page >> 27) & 511;
	uint64_t indices[1] = (page >> 18 ) & 511;
	uint64_t indices[2] = (page >> 9) >> 511;
	uint64_t indices[3] = page >> 511;
	int allocation;

	//create first table
	if (table_root == NULL){
		//create new table
		allocation = allocate_frame(number);
		if (allocation != -1){
			table_root = &memory[0] + allocation * 4096;
			memset(table_root,0,512 * sizeof(entry_t_));
			temp_root = mapHelper(table_root,number,indices[0]);
			if (!temp_root) return 0;
		}
		else return 0;
	}
	else{ //traverse
		temp_root = mapHelper(table_root,number,indices[0]);
		if (!temp_root) return 0;
	}
	//second table
	for (int i = 0; i < 2; i++) {
		temp_root = mapHelper(temp_root, number, indices[i+1]);
		if (!temp_root) return 0;
	}

	//if (temp_root[indices[3]].flags & 1) return -1;
	// QUESTION:WHAT IF THE LAST TABLE ENTRY IS USED?
	temp_root[indices[3]].address = frame;
	temp_root[indices[3]].flags = flags;

	return 1;
}

uint64_t mapHelper(uint64_t pointer, int number, uint64_t index){
	struct entry_t_ *temp_root;
	if (pointer[index].flags & 1){
		return pointer[index].address;
	}
	else{
		int allocation = allocate_frame(number);
		if (allocation==-1) return 0;
		temp_root = &memory[0] + allocation;
		memset(temp_root,0,512 * sizeof(entry_t_));
		pointer[index].address = (uint64_t)temp_root;
		pointer[index].flags = pointer[index].flags | 1;
		return (uint64_t)temp_root;
	}

	return 0;
}

uint64_t unmapHelper(uint64_t pointer, uint64_t index){
	struct entry_t_ *temp_root;
	if (pointer[index].flags & 1){
		return pointer[index].address;
	}
	else{
		return 0;
	}

	return 0;
}

int vm_unmap(uint64_t page, int number) {

	//QUESTION: CAN I CALL vm_map(page,0,number,0)
	//QUESTION: SHOULD WE SET every entry we traversed the flag to unused?
	struct entry_t_ *temp_root = table_root;
	uint64_t indices[4]; /// page: ___0__(28)___(9)___(9)___(9)___(9)
	uint64_t indices[0] = (page<<28) >> 55;
	uint64_t indices[1] = (page << 37 ) >> 55;
	uint64_t indices[2] = (page << 46) >> 55;
	uint64_t indices[3] = (page << 55) >> 55;
	int allocation;
	//create first table
	if (table_root == NULL){
		//create new table
	 	return 0;
	}
	else{ //traverse
		temp_root = unmapHelper(table_root,number,indices[0]);
		if (!temp_root) return 0;
	}
	//second table
	for (int i = 0; i < 2; i++) {
		temp_root = unmapHelper(temp_root, number, indices[i+1]);
		if (!temp_root) return 0;
	}

	temp_root[indices[3]].flags = temp_root[indices[3]].flags & (~1);
	// ~1 work?

	return 1;

}

/*uint64_t locate_helper(struct entry_t_ * temp_root, int number, int level, int * prev, uint64_t * index, int * n){
	for (int i = 0; i< 512; i++){
		if (!(temp_root.flags & 1)){ //this entry is unused
				if (level == 4){
					if (!(*prev)){ //previous is unused
						*prev = 1;
						*index += i;
					}
					(*n)++;
					if (*n == number) return *index;
				}
				else {
					return *index + i << ((4-level) * 9);
				}
		}

		else{ //the entry is used
			if (level != 4) return locate_helper(temp_root[i], number, level+1, index + i << ((4-level) * 9));
			else {
				*prev = 0;
				((*index) >> 9 )<< 9;
				*n = 0;
			}
		}
	}
    return -1;
}*/

uint64_t vm_locate(int number) {
	/*struct entry_t_ * temp_root = table_root;
	int index = 1;
	for (int i =0 ;i < 512;i++){
		if (temp_root[i].flags & 1 == 0){
			return table_root[i]; //QUESTION!
		}
		else{
			temp_root =
		}
	}*/
	int count = 0;
	int prev = 0;
	for (uint64_t = 0; i < 68719476736; i++){
		if (vm_translate(i)==-1){
			if (!prev){
				prev = 1;
				count++;
			}
			else count++;
			if (count = number) return i;
		}
		else{
			count = 0;
			prev = 0;
		}
	}
	return -1;
	//int * start = 0;
  //return locate_helper(table_root, number, 1, 0, start);
}

uint64_t vm_translate(uint64_t virtual_address) {

	struct entry_t_ *temp_root = table_root;
	uint64_t indices[4]; /// page: ___0__(28)___(9)___(9)___(9)___(9)
	uint64_t indices[0] = (virtual_address << 28) >> 55;
	uint64_t indices[1] = (virtual_address << 37) >> 55;
	uint64_t indices[2] = (virtual_address << 46) >> 55;
	uint64_t indices[3] = (virtual_address << 55) >> 55;

	for (int i=0;i<4;i++){
		if (temp_root[indices[i]].flags & 1 == 0) return -1;
		if (i == 3){
			return temp_root[indices[i]].address;
		}
		else{
			temp_root = temp_root[indices[i]].address;
		}
	}

	return -1;
}
