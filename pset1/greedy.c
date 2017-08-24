#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <math.h>


int main()
{
    double change = 0.0;
    
    do{
 
        printf("O hai! How much change is owed?\n");
        change = get_float();
        
    }while(change < 0);
    
    change = change * 100;
    change = round(change);
    int rounded = (int)change;
    int returned_coins = 0;
    
    while(rounded >= 25)
    {
        returned_coins++;
        rounded = rounded - 25;
    }
    
    while(rounded >= 10)
    {
        returned_coins++;
        rounded = rounded - 10;
    }
    
    while(rounded >= 5)
    {
        returned_coins++;
        rounded = rounded - 5;
    }
    
    while(rounded >= 1)
    {
        returned_coins++;
        rounded = rounded - 1;
    }
    
    printf("%i\n", returned_coins);
    
}