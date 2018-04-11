#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c_ext.h"
#include "pca9532.h"

#define DEVICE "/dev/i2c-0"

int main(int argc, char* argv[])
{
    __uint16_t parameters[argc];
    for(int i = 1; i < argc; i++)
    {
        long int temp = strtol(argv[i], NULL, 16);
        if(temp > 0xff)
        {
            printf("Argument %d is invalid\n", i);
            return 1;
        }
        else
        {
            parameters[i-1] = (__uint16_t)temp;
            //printf("Parameter %d is: %x\n", i, parameters[i-1]);
        }
    }

    if(parameters[0] && 0x01)
    {
        //Read value from adress
    }
    else
    {
        //Write value(s) to adress
    }

    return 0;
}
