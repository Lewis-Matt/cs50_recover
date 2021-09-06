#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;
int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: Program SOURCE\n");
        return 1;
    }

    // Open image directory (SD card)
    FILE *source = fopen(argv[1], "r");
    if (source == NULL)
    {
        printf("Could not open source: %s\n", argv[1]);
        return 2;
    }

    BYTE buffer[512];
    int img_counter = 0;
    FILE *recovered_img = NULL;
    int is_jpg = 0;
    // 3 chars for ###, 4 chars for .jpg, 1 char for \0
    char filename[8];


    // Read 512 Bytes from source, stored into buffer, once it has been through the 512 bytes 1 time (== 1) the loop terminates
    // This could also be (buffer, sizeof(BUFFER),1,source) == 1
    while(fread(buffer, sizeof(BYTE), 512, source) == 512)
    {
        // Looking for start of a jpeg file
        // (buffer[3] & 0xf0) == 0xe0 just looks at first 4 bits of this byte, and set remaining 4 bits to 0, The "&" is the bitwise operator
        // essentially a truth table for comparing two bits. Bit A and Bit B; A & B: 0 & 0 = 0, 0 & 1 = 0, 1 & 1 = 1, etc.
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // JPEG has been found
            if (is_jpg == 1)
            {
                // Found new picture, close current image
                fclose(recovered_img);
            }

            // Is it the first jpeg found? Set to 1 (note = and not ==)
            else
            {
                is_jpg = 1;
            }

            // Write ###.jpg, %03i.jpg means 3-digit integer. The %03i is called a "conversion specification" which specifies format
            // sprintf prints a formatted string to a location in memory (filename) large enough to hold all chars
            sprintf(filename, "%03i.jpg", img_counter);
            // Saves the recovered_img file as the ###.jpg
            recovered_img = fopen(filename, "w");
            img_counter++;
        }
            // New jpg, reads from buffer and writes to recovered_img
            if (is_jpg == 1)
            {
                fwrite(&buffer, sizeof(BYTE), 512, recovered_img);
            }
    }

    // Close all files
    fclose(source);
    if (recovered_img != NULL)
    {
        fclose(recovered_img);
    }
    return 0;
}
