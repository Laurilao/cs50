#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <cs50.h>

#include "bmp.h"

int main(int argc, string argv[])
{
    
    //Input checking and error print on incorrect usage
    if(argc != 3)
    {
        fprintf(stderr, "Error! Usage: ./whodunit <input.file> <output.file>\n");
        return 1;
    }
    
    //Save pointers for input and output files
    char *infile = argv[1];
    char *outfile = argv[2];
    
    //Try to open input file
    FILE *inptr = fopen(infile, "r");
    if(inptr == NULL)
    {
        fprintf(stderr, "Error, could not open file\n");
        return 2;
    }
    
    //Try to open output file
    FILE *outptr = fopen(outfile, "w");
    if(outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Error, could not write to output.file\n");
        return 3;
    }
    
    //Read input file's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    
    //Read input file's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    //Ensure file is 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            //Modify the infile's rgb triple
            if(triple.rgbtRed == 0xff)
            {
                triple.rgbtRed = 0x00;
            }
            if(triple.rgbtRed == 0x00 && triple.rgbtGreen == 0x00 && triple.rgbtBlue == 0x00)
            {
                triple.rgbtRed = 0xff;
                triple.rgbtGreen = 0xff;
                triple.rgbtBlue = 0xff;
            }
            if(triple.rgbtRed == 0x00 && triple.rgbtGreen == 0xff && triple.rgbtBlue == 0xff)
            {
                triple.rgbtRed = 0x00;
                triple.rgbtBlue = 0x00;
                triple.rgbtGreen = 0x00;
            }

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;

    
    
}