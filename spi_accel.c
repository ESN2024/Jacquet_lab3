#include <stdio.h>
#include "system.h"
#include "sys/alt_sys_init.h"
#include <io.h>
#include <alt_types.h>
#include <sys/alt_irq.h>
#include "opencores_i2c.h" // Assurez-vous d'inclure la bonne bibliothèque pour les fonctions I2C.
#include "altera_avalon_pio_regs.h"
#include <stdlib.h>

volatile __uint8_t m,u,d,c,negatif,bouton,j;
volatile __int16_t X, Y, Z;
volatile __int8_t device, regx0 , regy0, regz0, regx1 , regy1, regz1, datax0=0, datay0=0, dataz0=0, datax1=0, datay1=0, dataz1=0;
volatile __int8_t offsetx0=0, offsety0=0, offsetz0=0, offsetx1=0, offsety1=0, offsetz1=0;
volatile __int8_t x0 = 0, x1 = 0, y0 =0, y1 =0, z0 =0, z1 = 0;


__int16_t Traitement_negatif(__int16_t number, __uint8_t *negatif){ 
    if (number & 0x8000) {
        number = -(number) + 1;
        *negatif = 1;
    } else {
        *negatif = 0; 
    }
    return number;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Affichage(__int16_t X, __uint8_t *negatif) { 

    __uint8_t m,u,d,c;

    // Calcul BCD d'un nombre de 4 chiffres
    m = X / 1000;           
    c = (X / 100) % 10;     
    d = (X / 10) % 10;      
    u = X % 10;             

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, u); // affiche l'unite sur le digit0

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE, d); // affiche les dizaine sur le digit1

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_2_BASE, c); // affiche les centaines sur le digit2

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_3_BASE, m); // affiche les millieme sur le digit3
}
static void irqhandler_bouton_key1(void* context, alt_u32 id)
{
    j = j+1;
    if (j>5){
        j=0;
    }
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_5_BASE, 0x1);
}
int main()
{
    __uint8_t m,u,d,c;  
   
    I2C_init(OPENCORES_I2C_0_BASE, 50000000, 400000);
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_5_BASE, 0x1);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_5_BASE, 0x1);
    alt_irq_register(PIO_5_IRQ, NULL, irqhandler_bouton_key1);
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

        X = (x1 << 8) | x0;
        Y = (y1 << 8) | y0;
        Z = (z1 << 8) | z0;

 
        if (negatif) {
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_4_BASE, 0b1111); 
        } 
        else {
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_4_BASE, 0b1011); 
        }

        alt_printf("x = %x, y = %x, z = %x\n", X, Y, Z);
        
        if (j==0){
        X = (Traitement_negatif(X,&negatif))*3,9; 
        Affichage(X, &negatif);
        }
        if (j==2){
        Y = (Traitement_negatif(Y,&negatif))*3,9; 
        Affichage(Y, &negatif);
        }
        if (j==4){
        Z = (Traitement_negatif(Z,&negatif))*3,9; 
        Affichage(Z, &negatif);
        }
        alt_printf("%x",j);
        usleep(1000000);
    }
    return 0;
}
