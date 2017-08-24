#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, string argv[])
{
    
    string word = argv[1];
    int table_index = 0;
    
    for(int i = 0; i < strlen(word); i++)
    {
        printf("%c", word[i]);
    }
    
    printf("\n");

    if(isupper(word[0]))
    {
        table_index = word[0] % 65;
    }
    
    else if(islower(word[0]))
    {
        table_index = word[0] % 97;
    }
    
    printf("%i\n", table_index);
    
    return 0;
}

