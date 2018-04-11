#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c_ext.h"
#include "pca9532.h"

#define DEVICE "/dev/i2c-0"

int main(int argc, char* argv[])
{
    __uint8_t command = strtol(argv[1], NULL, 16) & 0x01;
    __uint8_t slaveAddress = strtol(argv[1], NULL, 16) >> 1;
    __uint8_t registerAddress = strtol(argv[2], NULL, 16);
    __uint8_t parameters[argc-3];
    int parameterCount;
    for(parameterCount = 0; parameterCount < argc-3; parameterCount++)
    {
        long int temp = strtol(argv[parameterCount+3], NULL, 16);
        if(temp > 0xff)
        {
            printf("Argument %d is invalid\n", parameterCount);
            return 1;
        }
        else
        {
            parameters[parameterCount] = (__uint8_t)temp;
        }
    }

    int dev;

    /* Open the i2c master device */
    if ((dev = open(DEVICE, O_RDWR)) < 0)
    {
        perror("Can't open device\n");
        return 1;
    }

    /* Select the right slave for our master to talk to*/
    if (i2c_select_slave(dev, slaveAddress))
    {
        perror("Can't select device\n");
        close(dev);
        return 1;
    }
    else printf("Slave with address: %x selected\n", slaveAddress);

    /*Check last bit of the I2C adress to read the command*/
    if(command)
    {
        /*Read value from adress*/
        __uint8_t values[parameters[0]];
        i2c_smbus_read_i2c_block_data(dev, registerAddress, parameters[0], values);
         printf("Reading %d bytes from start address %x:\n", parameters[0], registerAddress);
        int j;
        for(j = 0; j < parameters[0]; j++)
        {
            printf("%x\n", values[j]);
        }
    }
    else
    {
        /*Write value(s) to adress*/
        i2c_smbus_write_block_data(dev, registerAddress, parameterCount, parameters);
        printf("Bytes written to start address %x:\n", registerAddress);
        int i;
        for(i = 0; i < parameterCount; i++)
        {
            printf("%x\n", parameters[i]);
        }
    }

    return 0;
}
