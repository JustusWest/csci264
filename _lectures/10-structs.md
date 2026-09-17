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

## Structs and Functions

<details class="code-example" markdown="1">
<summary>Show code: a full program using structs</summary>

```c
#include <stdio.h>
#include <string.h>

/* define a new struct type (outside function bodies) */
struct studentT {
    char  name[64];
    int   age;
    float gpa;
    int   grad_yr;
};

/* function prototypes */
int checkID(struct studentT s1, int min_age);
void changeName(char *old, char *new);

int main(void) {
    int can_vote;
    // declare variables of struct type:
    struct studentT student1, student2;

    // access field values using .
    strcpy(student1.name, "Ruth");
    student1.age = 17;
    student1.gpa = 3.5;
    student1.grad_yr = 2030;

    // structs are lvalues
    student2 = student1;
    strcpy(student2.name, "Frances");
    student2.age = student1.age + 4;

    // passing a struct
    can_vote = checkID(student1, 18);
    printf("%s %d\n", student1.name, can_vote);

    can_vote = checkID(student2, 18);
    printf("%s %d\n", student2.name, can_vote);

    // passing a struct field value
    changeName(student2.name, "Kwame");
    printf("student 2's name is now %s\n", student2.name);

    return 0;
}

int checkID(struct studentT s, int min_age) {
    int ret = 1;

    if (s.age < min_age) {
        ret = 0;
        // changes age field IN PARAMETER COPY ONLY
        s.age = min_age + 1;
    }
    return ret;
}

void changeName(char *old, char *new) {
    if ((old == NULL) || (new == NULL)) {
        return;
    }
    strcpy(old,new);
}
```

</details>

```text
Ruth 0
Frances 1
student 2's name is now Kwame
```

**Structs are passed to functions by value**, like everything else in C. The parameter `s` is a full copy of the argument. `checkID` assigns to `s.age`, and that change affects the copy in `checkID`'s stack frame only.

**`changeName` behaves differently because of the type of the field.** `student2.name` is an array of `char`, so passing it passes the array's base address. The parameter `old` and the field in `main`'s struct refer to the same memory, so the `strcpy` inside the function changes the caller's struct.

## Pointers to Structs

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

One `struct studentT` variable holds one student. A roster holds many, and the number of students is not known until the program runs — so it goes on the heap, the same way any other array of unknown size does.

`struct studentT` is a type like any other, so `sizeof` works on it and `malloc` has everything it needs:

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
<p class="board-title">Example 1 — two spellings, one struct</p>

`roster` holds the address of the first struct, so indexing works exactly as it does for an array of `int`:

| Expression | Type | Field access |
|---|---|---|
| `roster[i]` | `struct studentT` — a whole struct | `roster[i].age` |
| `&roster[i]` | `struct studentT *` — a pointer to one | `ptr->age` |

`roster[2].age` and `(&roster[2])->age` name the same `int` at the same address. Which one you write depends on what you are holding: in `main` you have the array, so you use `.`; in a function that was handed one student, you have a pointer, so you use `->`.

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

Every field is reached with `->`, but they are not all assigned the same way. `age`, `gpa`, and `grad_yr` are numbers, so a plain `=` copies the value. `name` is an *array* of `char` sitting inside the struct, and you cannot assign to an array — it takes a string copy. The type of the field decides the rules, not the fact that it is a field.

The terminator line is not decoration. If someone passes a name 64 characters or longer, `strncpy` fills all 64 bytes and stops with no room left for `'\0'`, exactly as on Wednesday. Every `printf("%s", s->name)` after that would run off the end of the field — and the next thing in memory is `age`.

<details class="code-example" markdown="1">
<summary>Show code: the whole roster program</summary>

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

    init_student(&roster[0], "Kwame Salter",    20, 3.5, 2028);
    init_student(&roster[1], "Frances Allen",   19, 3.9, 2029);
    init_student(&roster[2], "Ruth Teitelbaum", 21, 3.2, 2027);
    init_student(&roster[3], "Lars Bak",        20, 2.8, 2028);
    init_student(&roster[4], "Freya Hansen",    18, 4.0, 2030);

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
    printf("%-16s age %d  gpa %.1f  class of %d\n",
        s->name, s->age, s->gpa, s->grad_yr);
}
```

```text
Kwame Salter     age 20  gpa 3.5  class of 2028
Frances Allen    age 19  gpa 3.9  class of 2029
Ruth Teitelbaum  age 21  gpa 3.2  class of 2027
Lars Bak         age 20  gpa 2.8  class of 2028
Freya Hansen     age 18  gpa 4.0  class of 2030
```

</details>

One `malloc` and one `free` for the whole roster. The five structs are not five allocations — they are five neighborhoods inside a single block, which is why `roster[3]` can be found by arithmetic instead of by bookkeeping.

The file is [`lecture_10.c`]({{ "/code/Chapter_2/lecture_10.c" | relative_url }}). Bring it to lab on Tuesday: **Lab 4** picks this program up and adds two functions to it — one that searches the roster for a student by name, and one that reports where each field actually sits inside a struct. The second is worth doing now because those offsets come back in November: when we read the assembly the compiler writes for `s->age`, the number 64 is going to be sitting in the instruction.

## Looking Ahead

For some practice, try the Exercises from the book for [§1.6](https://diveintosystems.org/exercises/section-1_6.html) and [§2.7](https://diveintosystems.org/exercises/section-2_7.html).
