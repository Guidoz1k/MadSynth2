#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <math.h>

#define TAB		16
#define MAX		65535
#define SIZE	2048
#define PI		3.14159265

int main(){
	int32_t counter = 0;
	int32_t integer = 0;
	float temp_number = 0;

	int32_t option = 2; // 1 = sine, 1 = square, 2 = sawtooth (phased), 3 = triangle
						// 4 = inverted sawtooth (phased), 5 = sawtooth, 6 = inverted sawtooth 

	for(counter = 0; counter < SIZE; counter++){
		switch(option){
			case 0:
				temp_number = sin( ((2*PI)/SIZE) * counter );
				integer = (MAX / 2) * temp_number;
				break;
			case 1:
				if(counter < (SIZE/2))
					integer = MAX / 2;
				else
					integer = - MAX / 2;
				break;
			case 2:
				integer = counter * (float)((float)(MAX - 1) / (float)(SIZE - 1));
				if(counter >= (SIZE/2))
					integer -= MAX;
				break;
			case 3:
				if(counter < (SIZE/4)){
					temp_number = counter * (float)((float)(MAX / 2) / (float)(SIZE / 4));
					integer = temp_number;
				}
				if((counter >= (SIZE/4)) && (counter < (3*SIZE/4))){
					temp_number = (counter - (SIZE/4)) * (float)((float)( MAX / 2) / (float)(SIZE / 4));
					integer = (MAX / 2) - temp_number;
				}
				if(counter >= (3*SIZE/4)){
					temp_number = (counter - (3*SIZE/4)) * (float)((float)(MAX / 2) / (float)(SIZE / 4));
					integer = - (MAX / 2) + temp_number;
				}
				break;
			case 4:
				integer = - counter * (float)((float)(MAX - 1) / (float)(SIZE - 1));
				if(counter >= (SIZE/2))
					integer += MAX;
				break;
			case 5:
				integer = - (MAX / 2) + counter * (float)((float)(MAX - 1) / (float)(SIZE - 1));
				break;
			case 6:
				integer = + (MAX / 2) - counter * (float)((float)(MAX - 1) / (float)(SIZE - 1));
				break;
			default:
				break;
		}
		if((counter % TAB) == 0)
			printf("\n");
		printf("%6d, ", integer);
	}
	getch();

}
