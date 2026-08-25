#include <stdio.h>

int main(void){
	int y;

	printf("Enter an integer: ");
	scanf("%d", &y);

	printf("decimal:    %d\n", y);
	printf("octal:    %o\n", y);
	printf("hexadecimal:    %x\n", y);

	return 0;

}
