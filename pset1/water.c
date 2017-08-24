#include <stdio.h>
#include <cs50.h>

int main()
{
    printf("Minutes: ");
    int minutes = get_int();
    int bottles = minutes * 12;
    printf("Bottles: %d\n", bottles);
}