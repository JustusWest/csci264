#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){

	char str1[11];
	char *str2 = NULL;
	char str3[50];	

	strcpy(str1, "Hello");

	int size = strlen(str1);
	printf("%d\n", size);

	str2 = malloc(sizeof(char) * (size + 1));

	strcpy(str2, str1);

	printf("%s \n", str1);
	printf("%s \n", str2);

	strcat(str1, str2);

	printf("%s \n", str1);

	strncpy(str3, str1, 10);
	printf("%s \n", str3);

	int comp = strcmp(str1, str3);
	printf("%d \n", comp);

	if(strcmp(str1, str3) == 0){
		printf("Equal \n");
	} else {
		printf("Not equal \n");
	}

	char str4[30];
	char str5[30];

	strcpy(str4, "alligator");
	strcpy(str5, "Zebra");
	comp = strcmp(str4, str5);
	
	if(comp < 0){
		printf("str4 is less\n");
	} else {
		printf("str4 is greater\n");
	}
	
	char *substr = strstr(str4, "gator");

	printf("substring value: %s \n", substr);
	printf("substring addr: %p \n", substr);
	printf("substring starts at: %p \n", &str4[4]);	

	printf("comp is %d \n", comp);

	return 0;
}
