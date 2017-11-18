/*
 * test_frame.c
 *
 *  Created on: Nov 10, 2017
 *      Author: hazhou
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "frame.h"

int main(){

	frame_init();

	uint64_t haha= allocate_frame(24);
	uint64_t hhh = allocate_frame(2);
	uint64_t aha = allocate_frame(6);
	int lol = deallocate_frame(4,8);
	int lmao = deallocate_frame(11, 10);

	uint64_t test = allocate_frame(5);
	uint64_t test2 = allocate_frame(20);
	printf("%lld\n",haha); //0
	printf("%lld\n",hhh); //24
	printf("%lld\n",aha); //26
	printf("%d\n",lol);  //4
	printf("%d\n",lmao); //11
	printf("%lld\n",test);  //4
	printf("%lld\n", test2);  //32


}
