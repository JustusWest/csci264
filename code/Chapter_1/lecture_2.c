#include <stdio.h>
#include <math.h>
#include <string.h>
int main(void) {
	    int num1, num2;
    printf("Enter the 1st number: ");
    scanf("%d", &num1);
    printf("Enter the 2nd number: ");
    scanf("%d", &num2);

	if (num1 > num2) {
		printf("Num1 bigger\n");
	} else if (num2 > num1) {
		printf("Num2 bigger\n");
	} else {
		printf("Equal\n");
	}

	if (-1) {
		printf("True\n");
	}

	if (0 || num1++){}
	printf("%d\n", num1);
	
	for(int i =0; i < num1; i++){
		printf("i:%d\n", i);
	}
	
	int j = 0;
	while (j < num1){
		printf("j:%d\n", j);
		j++;
	}

	int k = 0;
	do {
		printf("k:%d\n", k);
		k++;
	} while (k < num1);
	
	int l, size = 0;
	int arr[10];

	for(int i = 0; i < 10; i++){
		arr[i] = i;
		size++;
	}
	
	printf("array of %d items:\n", size);
	for (int i = 0; i < 10; i++) {
        	printf("%d\n", arr[i]);
	}
	char str1[10];
	char str2[20];
	int len;
	str1[0] = 'h';
	str1[1] = 'i';
	str1[2] = '\0';
	len = strlen(str1);
	printf("%s %d\n", str1, len);
	return 0;
}
