#include <stdio.h>
#include <stdlib.h>

int main(void){
	int n;
	int *arr = NULL;	

	printf("How many values? ");
	scanf("%d", &n);	

	arr = malloc(sizeof(int) * n);
	if(arr == NULL){
		printf("Error - malloc failed \n");
		exit(1);
	}

	printf("Enter %d values: ", n);
	for(int i = 0; i < n; i++){
		scanf("%d", &arr[i]);
	}
	
	printf("Value: ");
	for(int i = 0; i < n; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");

	printf("[main]    arr    = %p\n", arr);
	printf("[main]    &arr    = %p\n", &arr);

	printf("[main]    &arr[0]    = %p\n", &arr[0]);
	printf("[main]    &arr[1]    = %p\n", &arr[1]);

	free(arr);
	arr = NULL;


	return 0;
}
