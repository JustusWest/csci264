---
title: "10 Structs"
published: True
---

[*Reading: Dive into Systems §1.6*](https://diveintosystems.org/book/C1-C_intro/structs.html) and [*§2.7*](https://diveintosystems.org/book/C2-C_depth/structs.html)

Arrays create an ordered collection of data elements **of the same type**. Structs create a collection of data elements **of different types**, treated as a single unit.

C is not object-oriented and has no classes. A struct is the closest thing the language gives us for representing a heterogeneous collection of data.

## Defining a struct Type

Defining a struct adds a new type to C's type system. The definition goes *outside* any function body, generally near the top of the `.c` file.

<details class="code-example" markdown="1">
<summary>Show code: defining a struct type</summary>

```c
struct studentT {
    char name[64];
    int age;
    float gpa;
    int grad_yr;
};
```

</details>

The name of the new type is `struct studentT`. This defines a *type*, not a variable: no memory is set aside until something is declared.

## Declaring Variables and Assigning Fields

Declare variables of the new type the same way you declare any other:

```c
struct studentT student1, student2;
```

Access a field with **dot notation**. When you do, think carefully about the type of the field you are touching, because the rules that apply are the rules for *that* type — `name` is an array of `char`, so it needs `strcpy`, while `age` is an `int` and takes a plain assignment.

<details class="code-example" markdown="1">
<summary>Show code: assigning field values</summary>

```c
// The 'name' field is an array of characters, so we can use the 'strcpy'
// string library function to fill in the array with a string value.
strcpy(student1.name, "Kwame Salter");

// The 'age' field is an integer.
student1.age = 18 + 2;

// The 'gpa' field is a float.
student1.gpa = 3.5;

// The 'grad_yr' field is an int
student1.grad_yr = 2020;
student2.grad_yr = student1.grad_yr;
```

</details>

![The student1 struct drawn as a stack of labeled rows: the field names name, age, gpa, and grad_yr run down the left, and each names a region of memory to its right. The name row is subdivided into single-character boxes holding 'K', 'w', 'a', 'm', 'e', and a space; the age, gpa, and grad_yr rows are single boxes holding 20, 3.5, and 2020.]({{ "/figures/ch1/studentstruct.png" | relative_url }})

The variable is one block of memory, and the fields are named regions inside it, laid out in the order they were declared.

## Structs Are lvalues

A struct is an **lvalue** — it can appear on the left side of an assignment. Assigning one struct to another copies all of its field values.

```c
student2 = student1;  // student2 gets the value of student1
                      // (student1's field values are copied to
                      //  corresponding field values of student2)

strcpy(student2.name, "Frances Allen");  // change one field value
```

This is a copy not a shared reference. After the assignment, changing `student2.name` leaves `student1.name` alone.

## Structs, Functions and Pointers

**Structs are passed to functions by value**, like everything else in C. The parameter `s` is a full copy of the argument. 

If we want to modify a struct in a function, we pass a pointer to the struct.

A pointer to a struct is declared like a pointer to anything else, and the struct it points to can live on the stack or the heap.

<details class="code-example" markdown="1">
<summary>Show code: declaring and allocating a struct pointer</summary>

```c
struct studentT s;
struct studentT *sptr;

// think very carefully about the type of each field when
// accessing it (name is an array of char, age is an int ...)
strcpy(s.name, "Freya");
s.age = 18;
s.gpa = 4.0;
s.grad_yr = 2020;

// malloc space for a struct studentT for sptr to point to:
sptr = malloc(sizeof(struct studentT));
if (sptr == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
}
```

</details>

`sizeof(struct studentT)` gets the size of the whole struct so one `malloc` call is enough.

To reach a field through a pointer, dereference the pointer first. The classic syntax works:

```c
// the grad_yr field of what sptr points to gets 2021:
(*sptr).grad_yr = 2021;

// the age field of what sptr points to gets s.age plus 1:
(*sptr).age = s.age + 1;
```
However, because this pattern comes up constantly, C provides an operator that dereferences a struct pointer and accesses a field in one step: **`->`**

```c
// the gpa field of what sptr points to gets 3.5:
sptr->gpa = 3.5;

// the name field of what sptr points to is a char *
// (can use strcpy to init its value):
strcpy(sptr->name, "Lars");
```

`sptr->gpa` and `(*sptr).gpa` mean exactly the same thing. Use the arrow.

## An Array of Structs

One `struct studentT` variable holds one student. If we want to create a class roster, we need to store many students; we will do this with an array of structs. The total number isn't known until runtime so it goes on the heap, the same way any other array of unknown size does.

`struct studentT` is a type like any other, so we can use `sizeof` to `malloc` it:

```c
int n = 5;
struct studentT *roster = NULL;

roster = malloc(sizeof(struct studentT) * n);
if (roster == NULL) {
    printf("Error - malloc failed\n");
    exit(1);
}
```

Compare this to `malloc(sizeof(int) * n)` from Monday. The only thing that changed is the type inside `sizeof`. `malloc` returns one contiguous block big enough for five whole structs, and `roster` holds its base address.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — Two ways to access a field</p>

`roster` holds the address of the first struct, so indexing works the same as any array:

| Expression | Type | Field access |
|---|---|---|
| `roster[i]` | `struct studentT` — a whole struct | `roster[i].age` |
| `&roster[i]` | `struct studentT *` — a pointer to a struct | `ptr->age` |

`roster[2].age` and `(&roster[2])->age` name the same `int` at the same address. Which one we use depends on what we have: in `main` you have the array, so you use `.`; in a function that was handed one student, you have a pointer, so you use `->`.

</div>

### Initializing One Student

Write the function that fills in a single student first, and `main` becomes a list of calls to it. It takes a **pointer** to a struct that already exists — it does not allocate anything and it does not return anything.

```c
#define NAME_LEN 64   /* so the array size has a name we can reuse */

void init_student(struct studentT *s, char *name, int age, float gpa, int grad_yr) {
    strncpy(s->name, name, NAME_LEN);
    s->name[NAME_LEN - 1] = '\0';   /* strncpy did not do this for us */

    s->age = age;
    s->gpa = gpa;
    s->grad_yr = grad_yr;
}
```

Every field is reached with `->`, but they are not all assigned the same way. `age`, `gpa`, and `grad_yr` are numbers, so a plain `=` copies the value. `name` is an *array* of `char`, so we have to use `strcpy`. In this case we use `strncpy` to enforce a max size. We include the `'\0'` incase someone passes a name 64 characters or longer, where `strncpy` woulld fill all 64 bytes and stops with no room left for `'\0'`

<details class="code-example" markdown="1">
<summary>Show code: init Roster</summary>

```c
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

int main(void) {
    int n = 5;
    struct studentT *roster = NULL;

    roster = malloc(sizeof(struct studentT) * n);
    if (roster == NULL) {
        printf("Error - malloc failed\n");
        exit(1);
    }

    init_student(&roster[0], "Falilou",    20, 3.5, 2028);
    init_student(&roster[1], "Jamiya",   19, 3.9, 2029);
    init_student(&roster[2], "Nick", 21, 3.2, 2027);
    init_student(&roster[3], "Winnie",        20, 2.8, 2028);
    init_student(&roster[4], "Juan",    18, 4.0, 2030);

    for (int i = 0; i < n; i++) {
        print_student(&roster[i]);
    }

    free(roster);
    roster = NULL;

    return 0;
}

void init_student(struct studentT *s, char *name, int age, float gpa, int grad_yr) {
    strncpy(s->name, name, NAME_LEN);
    s->name[NAME_LEN - 1] = '\0';   /* strncpy did not do this for us */

    s->age = age;
    s->gpa = gpa;
    s->grad_yr = grad_yr;
}

void print_student(struct studentT *s) {
    printf("%s age %d  gpa %.1f  class of %d\n",
        s->name, s->age, s->gpa, s->grad_yr);
}
```

```text
Falilou    age 20  gpa 3.5  class of 2028
Jamiya    age 19  gpa 3.9  class of 2029
Nick  age 21  gpa 3.2  class of 2027
Winnie         age 20  gpa 2.8  class of 2028
Juan     age 18  gpa 4.0  class of 2030
```

</details>

One `malloc` allocates a block for the whole roster, so one `free` clears it.

This will be a good starting point for Lab 4.

## Looking Ahead

For some practice, try the Exercises from the book for [§1.6](https://diveintosystems.org/exercises/section-1_6.html) and [§2.7](https://diveintosystems.org/exercises/section-2_7.html).
