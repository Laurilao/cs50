#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUF_SIZE 512

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Error, usage: ./recover <file>\n");
        return 1;
    }
    
    // remember filename
    char *infile = argv[1];
    
    // try to open infile, print error if invalid
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Error, invalid file\n");
        return 2;
    }
    
    
    // initialize amount of jpgs found to 0
    int foundCount = 0;
    
    // initialize filecounter to 000
    int fileCount = 0;
        
    // create 512 byte buffer for storing the memory card chunks
    uint8_t buffer[BUF_SIZE];
    
    // create filepointer for output files
    FILE *img = NULL;
    
    // Read 512 bytes from input until no more whole chunks can be found (indicating EOF)
    while (fread(buffer, BUF_SIZE, 1, inptr) == 1)
    {
        // if jpeg signature is found, start writing
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // if an existing file is open, close it first and create a new one
            if (foundCount > 0)
            {
                fclose(img);
                ++fileCount;
                char filename[8];
                sprintf(filename, "%03i.jpg", fileCount);
                img = fopen(filename, "w");
            }
            
            // if the first header, print without opening a new file
            else if (foundCount == 0)
            {
                ++foundCount;
                char filename[8];
                sprintf(filename, "%03i.jpg", fileCount);
                img = fopen(filename, "w");
                if (img == NULL)
                {
                    fprintf(stderr, "Error, could not create target file\n");
                    return 2;
                }
            }
            
        }
        
        // if not start of a new jpg but an existing one is found, write into it
        if (foundCount > 0)
        {
            fwrite(buffer, BUF_SIZE, 1, img);
        }
    }
    
    // close input and output files
    fclose(img);
    fclose(inptr);
    
    return 0;
    
}
