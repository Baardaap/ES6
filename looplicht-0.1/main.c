#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c_ext.h"
#include "pca9532.h"

#define DEVICE "/dev/i2c-0"

#define DEFAULT_DELAY 500000
#define MICROSECOND 10000000
#define PWM_MAX 255
#define INPUT_POWER_MAX 100

int main(int argc, char* argv[])
{
    int dev;

    /* Open the i2c master device */
    if ((dev = open(DEVICE, O_RDWR)) < 0)
    {
        perror("Can't open device");
        return 1;
    }

    /* Select the right slave for our master to talk to */
    if (i2c_select_slave(dev, PCA9532_ADDRESS))
    {
        perror("Can't select device");
        close(dev);
        return 1;
    }

    /* Setup the states */
    unsigned int i;
    __u8 states[16][2] = {
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED0(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED1(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED2(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED3(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED0(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED1(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED2(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED3(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
    };

    int input;
    int delay = DEFAULT_DELAY;
    int pwm = 255;
    printf("Enter the light delay in Hz:\n");
    if(scanf("%d", &input))
    {
        //convert Hz to Microseconds for the usleep;
        delay = (1/input)*MICROSECOND;
    }
    printf("Set delay is: %iHz\n", input);

    printf("Enter the light power in %%:\n");
    if(scanf("%d", &input))
    {
        pwm = (input)*(PWM_MAX/INPUT_POWER_MAX);
        i2c_smbus_write_byte_data(dev, PCA9532_LEDMODE_PWM0, pwm);
    }
    printf("Set power is: %i%%\n", input);


        
    for (i = 0; i < 16; i++)
    {
        if (i2c_smbus_write_byte_data(dev, states[i][0], states[i][1]))
        {
            printf("Couldn't set LED 0x%02X on register 0x%02X\n", (int)states[i][1],
                   (int)states[i][0]);
        }
        else
        {
            printf("Set LED 0x%02X on register 0x%02X\n", (int)states[i][1],
                   (int)states[i][0]);
        }

        usleep(delay);
    }

    close(dev);

    return 0;
}
