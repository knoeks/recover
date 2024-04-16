#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover file_name.raw\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("File could not be opened.\n");
        return 1;
    }
    //find first FAT block or find 0xff 0xd8 0xff

    unsigned char *buffer = malloc(512 * sizeof(char));
    char *file_counter = malloc(8 * sizeof(char));

    //JPG keys converted to two byte uints
    uint16_t key[] = {0xff, 0xd8, 0xff, 0xe0};

    int digit = 0;
    bool found_JPEG = false;
    FILE *output = malloc(sizeof(long));

    //here the JPEG begining should already be found
    while ((fread(buffer, 512, 1, f)) == 1) //fread outputs one when data was read
    {
        bool is_key_correct = false;
        int key_counter = 0;

        //file name creator
        sprintf(file_counter, "%.3i.jpg", digit);
        for (int i = 0; i < 4; i++)
        {
            //checks if first 3 keys correct
            if (buffer[i] == key[i])
            {
                key_counter++;
            }
            //checks if last 4th key is in the bounds
            else if ((buffer[i] & 0xf0) == key[i])
            {
                key_counter++;
            }
            if (key_counter == 4)
            {
                is_key_correct = true;
            }
        }

        if (is_key_correct == true)
        {
            if (digit == 0)
            {
                found_JPEG = true;
            }
            else
            {
                fclose(output);
            }
            output = fopen(file_counter, "w");
            if (output == NULL)
            {
                printf("File could not be opened.\n");
                return 1;
            }
            fwrite(buffer, 512, 1, output);
            digit++;
        }
        else
        {
            if (found_JPEG == true)
            {
                fwrite(buffer, 512, 1, output);
            }
        }

    }

    //make two loops: 1st for looking for signature 0xff 0xd8 0xff 0xe0 < lastkey < 0xef
    //then iterates as long as buffer doesnt contain signature
    free(buffer);
    free(file_counter);
    free(output);
    fclose(f);
    fclose(output);
}
