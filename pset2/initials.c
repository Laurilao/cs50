#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>


int main()
{
    string full_name = get_string();
    
    for(int i = 0, n = strlen(full_name); i < n; i++)
    {
        if(isalpha(full_name[i]) && i == 0)
        {
            printf("%c", toupper(full_name[0]));
        }
        
        if(isalpha(full_name[i]) && isspace(full_name[i - 1]))
        {
            printf("%c", toupper(full_name[i]));
        }
    }
    
    printf("\n");
    
}