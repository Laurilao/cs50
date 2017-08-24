#include <stdio.h>
#include <cs50.h>
#include <string.h>

void printSpace();
void printHash();
void leftPyramid(int, int);
void printGap();
void rightPyramid(int);


int main()
{
    int height = 25;
    
    while(height > 23 || height < 0)
    {
        printf("Height: ");
        height = get_int();
    }
    
    for(int rows = 0, n = height; rows < n; rows++)
    {
        leftPyramid(height, rows);
        printGap();
        rightPyramid(rows);
        printf("\n");
        height--;
    }
    
}

void leftPyramid(int height, int rows)
{
    for(int spaces = 0; spaces < height - 1; spaces++)
    {
        printSpace();
    }
    for(int hashes = 0; hashes < rows + 1; hashes++)
    {
        printHash();
    }
}

void rightPyramid(int rows)
{
    for(int hashes = 0; hashes < rows + 1; hashes++)
    {
        printHash();
    }
}

void printSpace()
{
    printf(" ");
}

void printHash()
{
    printf("#");
}

void printGap()
{
    printf("  ");
}