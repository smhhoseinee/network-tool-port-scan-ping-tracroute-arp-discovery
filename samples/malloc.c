
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main()
{
	int *ptr_one;

	ptr_one = (int *)malloc(sizeof(int)*1000000000);

	if (ptr_one == 0)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}
	memset(ptr_one,'\0',100);

	*ptr_one = 25;
	printf("%d\n", *ptr_one);

	int x;
	scanf("%d",&x);
	printf("%d\n", x);
	free(ptr_one);

	return 0;
}

