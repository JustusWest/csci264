#include <stdio.h>
#include <math.h>

int main(void) {
	char letter;
	letter = 'a';
	letter = letter + 1;
	printf("Letter: %d\n", letter);
	
	int y;
	y = pow(2, 32);
	printf("%d\n", y);

	int a, b;
	float c, d;

	a = 11;
	b = 2;
	a++;
	c = 11.0;
	d = 2.0;

	printf("Int division: %d\n", a/b);
        printf("Float division: %f\n", a/d);
        printf("Unsigned: %u\n", -110);

	scanf("%d", &a);
	printf("After scan: %d\n", a);


	 return 0;
	

}
