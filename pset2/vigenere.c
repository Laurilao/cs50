#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


int main(int argc, string argv[])
{
    if(argc != 2)
    {
        printf("Error, usage: ./vigenere <key>\n");
        return 1;
    }
    
    // Stores the argument variable in string 'key' for easier usage
    string key = argv[1];
    int keylength = strlen(key);
    
    // Checks the cipherkey for non-alphabetical characters
    for(int i = 0; i < keylength; i++)
    {
        if(!isalpha(key[i]))
        {
            printf("Error, enter all alphabetical characters\n");
            return 1;
        }
    }
    
    // Checks the casing on the cipherkey and converts to upper case by default
    // and makes it 0 indexed alphabetically
    for(int i = 0; i < keylength; i++)
    {
        if(islower(key[i]))
        {
            key[i] = toupper(key[i]);
        }
        
        key[i] = key[i] - 65;
    }
    
    
    printf("plaintext: ");
    string plaintext = get_string();
    string ciphertext = plaintext;
    
    // Ciphertext string index
    int j = 0;
 
    // Cipherkey length index
    int k = 0;
    
    int converted = 0;
    
    for(int i = 0, l = strlen(plaintext); i < l; i++)
    {
        
        if(isalpha(plaintext[i]))
        {
            int x = key[k];
            
            if(isupper(plaintext[i]))
            {
                converted = plaintext[i] - 65;
                ciphertext[j] = ((converted + x) % 26) + 65;
            }
            if(islower(plaintext[i]))
            {
                converted = plaintext[i] - 97;
                ciphertext[j] = ((converted + x) % 26) + 97;
            }
            
            j++;
            k++;
            
            k = k % keylength;
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