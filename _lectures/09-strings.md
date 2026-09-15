---
title: "09 Strings and the C String Library"
---

[*Reading: Dive into Systems §2.6*](https://diveintosystems.org/book/C2-C_depth/strings.html)

C has no string type. A string is an array of `char` that ends with the **null character**, `'\0'`. Standard string funcitons come from `string.h`

## Strings Are char Arrays

Not every `char` array is a string, but every C string is a `char` array. The terminator is what makes the difference: it marks where the characters stop.

This means the array must have capacity for the characters **plus one more**. To store `"hi"` you need at least three `char`s: `'h'`, `'i'`, and `'\0'`.

`printf` with the `%s` placeholder starts at the base address and prints characters until it reaches `'\0'`. The string library works the same way: its functions find the end of a string by searching for `'\0'`, and they add a `'\0'` to the end of any string they modify. If the terminator isn't there, they keep going past the end of your array.

C provides the `string.h` library to manipulate strings. Include it to use any of the functions below.

<details class="code-example" markdown="1">
<summary>Show code: building a string by hand</summary>

```c
#include <stdio.h>
#include <string.h>   // include the C string library

int main(void) {
    char str1[10];
    char str2[10];

    str1[0] = 'h';
    str1[1] = 'i';
    str1[2] = '\0';   // explicitly add null terminating character to end

    // strcpy copies the bytes from the source parameter (str1) to the
    // destination parameter (str2) and null terminates the copy.
    strcpy(str2, str1);
    str2[1] = 'o';
    printf("%s %s\n", str1, str2);  // prints: hi ho

    return 0;
}
```

</details>


## Dynamically Allocated Strings

Since strings are just `char` arrays, they can be dynamically allocated like any other array. Remember to include space for the terminator:

```c
new_str = malloc(sizeof(char) *  11);   // + 1 for '\0'
```

<details class="code-example" markdown="1">
<summary>Show code: static and dynamic strings</summary>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int size;
    char str[64];         // statically declared
    char *new_str = NULL; // for dynamically allocated

    strcpy(str, "Hello");
    size = strlen(str);   // returns 5

    new_str = malloc(sizeof(char) * (size+1)); // need space for '\0'
    if (new_str == NULL) {
        printf("Error: malloc failed!  exiting.\n");
        exit(1);
    }
    strcpy(new_str, str);
    printf("%s %s\n", str, new_str);    // prints "Hello Hello"

    strcat(str, " There");  // concatenate " There" to the end of str
    printf("%s\n", str);    // prints "Hello There"

    free(new_str);  // free malloc'ed space when done
    new_str = NULL;

    return 0;
}
```
</details>

Both `str` and `new_str` can be passed to any string function. A statically declared array and a `malloc`'d block both evaluate to a base address, which is all a `char *` parameter wants.

## Destination Memory
Many built in string functions (`strcpy`, `strcat`) store their results by following a destination pointer (char *)

The function assumes that the destination has adequate memory space - it is your job as the programmer to ensure this is true


```c
// Attempt to write a 12-byte string into a 5-character array.
char mystr[5];
strcpy(mystr, "hello world");

// Attempt to write to a string with a NULL destination.
char *mystr = NULL;
strcpy(mystr, "try again");

// Attempt to modify a read-only string literal.
char *mystr = "string literal value";
strcat(mystr, "string literals aren't writable");
```

Each of these compiles. The consequences range from a crash to silently corrupting whatever happened to sit after the array. The third one is worth noting on its own: a string *literal* is stored in a read-only part of program memory, so `char *mystr = "..."` gives you a pointer to memory you are not allowed to write. If you intend to modify a string, it needs to be in an array you declared or memory you allocated.

## strlen, strcpy, strncpy

```c
// returns the number of characters in the string (not including the null character)
int strlen(char *s);

// copies string src to string dst up until the first '\0' character in src
// (the caller needs to make sure src is initialized correctly and
// dst has enough space to store a copy of the src string)
// returns the address of the dst string
char *strcpy(char *dst, char *src);

// like strcpy but copies up to the first '\0' or size characters
char *strncpy(char *dst, char *src, size_t size);
```

The `n` versions of these functions take a maximum number of characters, which keeps them from running off the end of a destination that is too small. `size_t` is just an unsigned integer type.

**`strncpy` does not always null terminate.** If the source string is `size` characters or longer, `strncpy` copies exactly `size` characters and stops — with no room left for a terminator, it doesn't add one. You have to do it yourself:

```c
strncpy(d_str, str, 5);
d_str[5] = '\0';   // strncpy did not do this for you
```

Skip that line and you have an unterminated `char` array

## Comparing Strings

The `==` operator does not compare the characters of two strings. It compares the two values being held, which for arrays are **base addresses**.

<details class="code-example" markdown="1">
<summary>Show code: comparing strings</summary>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char str[32];
    char *d_str;

    strcpy(str, "Hello");

    d_str = malloc(sizeof(char) * (strlen(str) + 1));
    if (d_str == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }
    strcpy(d_str, str);

    printf("str   = %s\n", str);
    printf("d_str = %s\n", d_str);

    if (d_str == str) {
        printf("equal\n");
    } else {
        printf("not equal\n");
    }

    printf("str   holds %p\n", str);
    printf("d_str holds %p\n", d_str);

    free(d_str);
    return 0;
}
```

```text
str   = Hello
d_str = Hello
not equal
str   holds 0x7ffd80d6e530
d_str holds 0x56502824c2a0
```

Both strings contain the same five characters, but the variables are not equal. This is because both are pointers to the beginning of the array - they store memory addresses.
</details>
To compare the content of the strings, use `strcmp` or `strncmp`:

```c
// returns 0 if s1 and s2 are the same strings
// a value < 0 if s1 is less than s2
// a value > 0 if s1 is greater than s2
int strcmp(char *s1, char *s2);

// compare s1 and s2 up to at most n characters
int strncmp(char *s1, char *s2, size_t n);
```

The fix to the program above is one line:

```c
if (strcmp(d_str, str) == 0) {
    printf("equal\n");
}
```

Note that `strcmp` returns 0 when the strings *match*. If you put this in an if statement directly `if(strcmp(d_str, str))` it won't work, because 0 evaluates to `False`.

`strncmp` compares only the first `n` characters, which is how you ask whether two strings *start* the same way:

```c
strcpy(str, "alligator");
ret = strncmp(str, "all", 3);  // returns 0: they are equal up to first 3 chars
```

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — strcmp compares ASCII values</p>

`strcmp` walks both arrays and compares the `char` values at corresponding positions. Those are ASCII encodings, so the ordering it produces is *case sensitive* rather than alphabetical.

```c
strcpy(str, "alligator");
strcpy(d_str, "Zebra");

ret = strcmp(str, d_str);
if (ret == 0) {
    printf("%s is equal to %s\n", str, d_str);
} else if (ret < 0) {
    printf("%s is less than %s\n", str, d_str);
} else {
    printf("%s is greater than %s\n", str, d_str);  // true for these strings
}
```

`'a'` is 97 and `'Z'` is 90, so `"alligator"` comes out *greater* than `"Zebra"`.

</div>

## strcat and strstr

```c
// append chars from src to end of dst
// returns ptr to dst and adds '\0' to end
char *strcat(char *dst, char *src);

// locate a substring inside a string
// returns a pointer to the beginning of substr in string,
// or NULL if substr is not in string
char *strstr(const char *string, const char *substr);
```

When using `strcat` the destination must be large enough for **both** strings plus the terminator.

`strstr` returns a **pointer into the original string**, at the position where the substring starts — or `NULL` if it isn't there. Since a pointer into the middle of a string is itself a valid string (the same terminator ends it), you can print it directly:

```c
char *ptr;

strcpy(str, "alligator");
ptr = strstr(str, "gator");
printf("%s\n", ptr);          // prints: gator
```


For some practice, try the [Exercises from the book for §2.6](https://diveintosystems.org/exercises/section-2_6.html).
