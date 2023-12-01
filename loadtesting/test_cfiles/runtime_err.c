#include <stdio.h>

int main() {

    int i; // Variable definition

    for (i = 1; i <= 10; i++) // Iteration 10 times
    {
        printf("%d ", i / 0); // Print the number.
    }
    return 0;
}
