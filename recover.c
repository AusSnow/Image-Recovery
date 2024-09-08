#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Prints error if not one command line argument and returns 1
    if (argc != 2)
    {
        printf("Usage: ./recover memory\n");
        return 1;
    }

    // Initialize variables and open memory card
    FILE *card = fopen(argv[1], "r");
    FILE *img = NULL;
    char imgname[8];
    uint8_t buffer[512];
    int imgcount = 0;

    // While data is left to be read from the memory card
    while (fread(buffer, 1, 512, card) == 512)
    {
        // If first four bytes represent a jpg file
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // If first image, write block of memory to jpg file "000.jpg", and increase image
            // counter
            if (imgcount == 0)
            {
                sprintf(imgname, "%03i.jpg", imgcount);
                img = fopen(imgname, "w");
                if (img == NULL)
                {
                    printf("Error opening image.\n");
                    fclose(card);
                    return 1;
                }
                fwrite(buffer, 1, 512, img);
                imgcount++;
            }

            // If not first image, close previous image, write to "###.jpg", and increase image
            // counter
            else
            {
                fclose(img);
                sprintf(imgname, "%03i.jpg", imgcount);
                img = fopen(imgname, "w");
                if (img == NULL)
                {
                    printf("Error opening image.\n");
                    fclose(card);
                    return 1;
                }
                fwrite(buffer, 1, 512, img);
                imgcount++;
            }
        }

        // If first four bytes do not represent a jpg file continue writing to current file
        else
        {
            if (img != NULL)
            {
                fwrite(buffer, 1, 512, img);
            }
        }
    }

    // Close all files
    fclose(card);
    if (img != NULL)
    {
        fclose(img);
    }
}
