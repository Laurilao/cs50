#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


int main(int argc, string argv[])
{
    if(argc != 2)
    {
        printf("Enter 1 command-line argument\n");
        return 1;
    }
    
    //k = cipherkey entered at command-line
    int k = atoi(argv[1]);
    
    printf("plaintext: ");
    string plaintext = get_string();
    string ciphertext = plaintext;
    
    //Ciphertext string index
    int j = 0;
    
    //ASCII converted into alphabetical index
    int converted = 0;
    
    // Iterating over and enciphering the plaintext string
    for(int i = 0, n = strlen(plaintext); i < n; i++)
    {
        if(isalpha(plaintext[i]))
        {
            if(isupper(plaintext[i]))
            {
                converted = plaintext[i] - 65;
                ciphertext[j] = ((converted + k) % 26) + 65;
                j++;
            }
            if(islower(plaintext[i]))
            {
                converted = plaintext[i] - 97;
                ciphertext[j] = ((converted + k) % 26) + 97;
                j++;
            }
        }
        else
        {
            ciphertext[j] = plaintext[i];
            j++;
        }
        
    }
    
    
    printf("ciphertext: %s\n", ciphertext);
    return 0;
}