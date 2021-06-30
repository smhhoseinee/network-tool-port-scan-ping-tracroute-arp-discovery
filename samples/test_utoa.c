#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main( void )
{
	uint32_t a=243;
	int b=243;

	if(b==a){
		printf("b is gt a");
	}else{
		printf("b is lt a");
	}


	return( EXIT_SUCCESS );
}
