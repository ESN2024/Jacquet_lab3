#include <stdio.h>
#include "system.h"
#include "sys/alt_sys_init.h"
#include <io.h>
#include <alt_types.h>
#include <sys/alt_irq.h>
#include "opencores_i2c.h" // Assurez-vous d'inclure la bonne bibliothèque pour les fonctions I2C.
#include <stdio.h>
#include "system.h"
#include <io.h>
#include <alt_types.h>
#include "sys/alt_sys_init.h"
#include "opencores_i2c.h"
#include <stdio.h>
#include "system.h"
#include "sys/alt_sys_init.h"
#include <io.h>
#include <alt_types.h>
#include <sys/alt_irq.h>
#include "altera_avalon_pio_regs.h"
#include <stdlib.h>

volatile __int8_t device, regx0 , regy0, regz0, regx1 , regy1, regz1, datax0=0, datay0=0, dataz0=0, datax1=0, datay1=0, dataz1=0;
volatile __int8_t offsetx0=0, offsety0=0, offsetz0=0, offsetx1=0, offsety1=0, offsetz1=0;
volatile __int8_t x0 = 0, x1 = 0, y0 =0, y1 =0, z0 =0, z1 = 0;
int hexToSignedInt(const char *hexStr) {
    long val = strtol(hexStr, NULL, 16);

    // Si le nombre est supérieur à 127, il est considéré comme négatif.
    if (val > 0x7F) {
        val = -1 * ((~val & 0xFF) + 1);
    }

    return (int)val;
}

static void irqhandler_switch_key1(void* context, alt_u32 id)
{     
    offsetx0 = datax0;
    offsety0 = datay0;
    offsetz0 = dataz0;
    offsetx1 = datax1;
    offsety1 = datay1;
    offsetz1 = dataz1;
    alt_printf("reset");
    I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, 0x1E, 0);
    I2C_write(OPENCORES_I2C_0_BASE, offsetx0, 1);

    I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, 0x1F, 0);
    I2C_write(OPENCORES_I2C_0_BASE, offsety0, 1);

    I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, 0x20, 0);
    I2C_write(OPENCORES_I2C_0_BASE, offsetz0, 1);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x0);
}

void READ_DATA (__uint8_t device, __uint8_t regx0, __uint8_t regy0, __uint8_t regz0, __uint8_t regx1, __uint8_t regy1, __uint8_t regz1, __uint8_t *datax0, __uint8_t *datay0, __uint8_t *dataz0,__uint8_t *datax1, __uint8_t *datay1, __uint8_t *dataz1 ){
    I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, regx0, 0); // set command to read input register.
    I2C_start(OPENCORES_I2C_0_BASE, device, 1);
    *datax0 = I2C_read(OPENCORES_I2C_0_BASE, 1); // Use 1 as the argument to indicate a NACK on the last read
    I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, regy0, 0); // set command to read input register.
    I2C_start(OPENCORES_I2C_0_BASE, device, 1);
    *datay0 = I2C_read(OPENCORES_I2C_0_BASE, 1); // Use 1 as the argument to indicate a NACK on the last read
     I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, regz0, 0); // set command to read input register.
    I2C_start(OPENCORES_I2C_0_BASE, device, 1);
    *dataz0 = I2C_read(OPENCORES_I2C_0_BASE, 1); // Use 1 as the argument to indicate a NACK on the last read
    I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, regx1, 0); // set command to read input register.
    I2C_start(OPENCORES_I2C_0_BASE, device, 1);
    *datax1 = I2C_read(OPENCORES_I2C_0_BASE, 1); // Use 1 as the argument to indicate a NACK on the last read
     I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, regy1, 0); // set command to read input register.
    I2C_start(OPENCORES_I2C_0_BASE, device, 1);
    *datay1 = I2C_read(OPENCORES_I2C_0_BASE, 1); // Use 1 as the argument to indicate a NACK on the last read
     I2C_start(OPENCORES_I2C_0_BASE, device, 0);
    I2C_write(OPENCORES_I2C_0_BASE, regz1, 0); // set command to read input register.
    I2C_start(OPENCORES_I2C_0_BASE, device, 1);
    *dataz1 = I2C_read(OPENCORES_I2C_0_BASE, 1); // Use 1 as the argument to indicate a NACK on the last read
}


int main()
{
    // most devices work at 100Khz  some faster
    I2C_init(OPENCORES_I2C_0_BASE, 50000000, 400000);
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_0_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0x1);
    alt_irq_register(PIO_0_IRQ, NULL, irqhandler_switch_key1);
    while (1)
    {
        // Boucle principale
        device = 0x1D;
        regx0 = 0x32;
        regx1 = 0x33;
        regy0 = 0x34;
        regy1 = 0x35;
        regz0 = 0x36;
        regz1 = 0x37;
        READ_DATA(device,regx0,regy0,regz0,regx1,regy1,regz1,&datax0,&datay0,&dataz0,&datax1,&datay1,&dataz1);
        x0 = (datax0);
        x1 = (datax1);
        y0 = (datay0);
        y1 = (datay1);
        z0 = (dataz0);
        z1 = (dataz1);
        alt_printf("Data at address x = %x/%x, y = %x/%x, z = %x/%x\n", x1, x0, y1, y0, z1, z0);
        usleep(1000000);
    }
    return 0;
}



























