/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    //Convert resize-factor argument to an integer
    int n = atoi(argv[1]);
    
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize <factor> <infile> <outfile>\n");
        return 1;
    }
    else if(n <= 0 || n > 100)
    {
        fprintf(stderr, "Error, factor must be greater than 0 and less than or equal to 100\n");
        return 1;
    }
    

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // Update outfile's BITMAPINFOHEADER to resize by factor <n>, and save original values of infile
    int inWidth = bi.biWidth;        // Infile width
    bi.biWidth *= n;                 // Updated outfile width
    int inHeight = abs(bi.biHeight); // Infile Height
    bi.biHeight *= n;                // Updated outfile height
   
    // Determine padding of infile
    int paddingIn = (4 - (inWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    // Determine padding for resized outfile
    int paddingOut = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Update the the size in BITMAPINFOHEADER for outfile
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + paddingOut) * abs(bi.biHeight); 

    // Update outfile's BITMAPFILEHEADER to resize by factor <n>
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    //char *scanline;
    //scanline = malloc(sizeof(RGBTRIPLE) * bi.biWidth + sizeof(char) * padding + 1);

    // Linereset values for the inputfile's pointer
    int linereset = sizeof(RGBTRIPLE) * inWidth;
    
    // iterate over infile's scanlines
    for (int i = 0; i < inHeight; i++)
    {
        // iterate over infile's scanlines <n> times
        for(int m = 0; m < n; m++)
        {
            // iterate over pixels in infile's scanline
            for (int j = 0; j < inWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
    
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                // Write RGB triple <n> times to outfile
                for(int l = 0; l < n; l++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
        
            // add padding to outfile (if necessary)
            for (int k = 0; k < paddingOut; k++)
            {
                fputc(0x00, outptr);
            }
            
            // Set the infile cursor back n-1 times
            if(m < n - 1)
            {
                // Set the infile pointer back to the beginning of the current scanline
                fseek(inptr, -linereset, SEEK_CUR);
            }
        }
       
        // skip over padding in infile, if any
        fseek(inptr, paddingIn, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
