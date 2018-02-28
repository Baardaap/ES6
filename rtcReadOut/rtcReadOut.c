#include <stdio.h>
#include "stdint.h"

int main()
{
    int val;
    uint32_t *a = (uint32_t*) 0x40024000;
    val = *a;
    printf("%d", val);
}