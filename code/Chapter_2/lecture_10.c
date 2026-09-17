/* lecture_10.c -- an array of structs on the heap
 * CSCI 264, Fri 9/18.  Starter file for Lab 4.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 64

struct studentT {
	char name[NAME_LEN];
	int age;
	float gpa;
	int grad_yr;
};

void init_student(struct studentT *s, char *name, int age, float gpa, int grad_yr);
void print_student(struct studentT *s);

int main(void){
	int n = 5;
	struct studentT *roster = NULL;

	roster = malloc(sizeof(struct studentT) * n);
	if(roster == NULL){
		printf("Error - malloc failed \n");
		exit(1);
	}

	init_student(&roster[0], "Kwame Salter",   20, 3.5, 2028);
	init_student(&roster[1], "Frances Allen",  19, 3.9, 2029);
	init_student(&roster[2], "Ruth Teitelbaum", 21, 3.2, 2027);
	init_student(&roster[3], "Lars Bak",       20, 2.8, 2028);
	init_student(&roster[4], "Freya Hansen",   18, 4.0, 2030);

	for(int i = 0; i < n; i++){
		print_student(&roster[i]);
	}

	free(roster);
	roster = NULL;

	return 0;
}

/* fill in one student.  s points to a struct that already exists. */
void init_student(struct studentT *s, char *name, int age, float gpa, int grad_yr){
	strncpy(s->name, name, NAME_LEN);
	s->name[NAME_LEN - 1] = '\0';	/* strncpy did not do this for us */

	s->age = age;
	s->gpa = gpa;
	s->grad_yr = grad_yr;
}

void print_student(struct studentT *s){
	printf("%-16s age %d  gpa %.1f  class of %d\n",
		s->name, s->age, s->gpa, s->grad_yr);
}
