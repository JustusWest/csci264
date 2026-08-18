#include <stdio.h>

#define SHOW(expr) printf(#expr " = %d\n", (expr))  /* int expressions */
#define SHOWF(expr) printf(#expr " = %f\n", (expr)) /* float / double */
#define SHOWC(expr) printf(#expr " = %c\n", (expr)) /* char */
#define SHOWS(expr) printf(#expr " = %s\n", (expr)) /* string (char*) */

int main(void)
{
    int arr[10];

    arr[3] = 100;

    SHOW(arr[3]);
    SHOW(2 + 3 * 4);

    return 0;
}
