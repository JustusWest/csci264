#include <stdio.h>

int max(int x, int y){
	int bigger;
	if(x>y){
		bigger = x;
	}else{
		bigger = y;
	}
	return bigger;
}



int main(void) {
	int x, y, result;
	printf("Enter value 1: ");
	scanf("%d", &x);
	printf("Enter value 2: ");
	scanf("%d", &y);
	
	printf("x holds %d and lives at %p\n", x, &x);


	int *ptr1, *ptr2, a, b;
	
	a = 8;
	ptr2 = &a;
	ptr1 = NULL;

	*ptr2 = 10;
	b = *ptr2 + 3;
	
	ptr1 = ptr2;
	*ptr1 = 80;

	ptr1 = &b;
	*ptr1 = 100;

	result = max(x, y);
	printf("%d is the largest of %d and %d\n", result, x, y);
	return 0;
}
