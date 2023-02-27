/*
 * Fichier d'enêete de la bibliothèque I2C
*/
#ifndef _I2C_
#define _I2C_
#include <stdint.h>
/* Fonction d'initialisation
 * les arguments doivent être fixés pour obtenir une fréquence de 100 ou 400kHz
*/

//#define DEBUG

/* Commande du DS1621 :*/
#define  ReadTemperature  0xAA
#define  ReadCounter      0xA8
#define  ReadSlope        0xA9
#define  StartConvertT    0xEE
#define  StopConvertT     0x22
#define  AccessTH         0xA1
#define  AccessTL         0xA2
#define  AccessConfig     0xAC
#define  DS1621           0x90
#define PCF8574           0x4E
#define PCF8563           0xA2
#define PCF8576           0x70

#define   Seconds 1 //VL SECONDS (0 to 59)
#define   Minutes 2 //MINUTES (0 to 59)
#define   Hours 3   // HOURS (0 to 23)
#define   Days 4   // DAYS (1 to 31)
#define   Weekdays 5 //WEEKDAYS (0 to 6) 
#define   Century_months 6 //MONTHS (1 to 12)
#define   Years 7  //YEARS (0 to 99) 
#define   Noc 24 
#define ON 1
#define OFF 0

uint8_t initLCD(void);
/* Fonctions internes à la bibliothèque*/

int8_t I2C_action(int8_t command);

int8_t I2C_start(void);

void I2C_stop(void);

int8_t I2C_write_data(uint8_t);

int8_t I2C_read_data(int8_t);


/* Fonctions externes*/
void I2C_UEXT(uint8_t on);

void I2C_init(uint8_t bitrate, uint8_t prescaler);

int8_t I2C_write(const uint8_t  address, uint8_t *data, uint8_t n);

int8_t I2C_read(const uint8_t address, uint8_t *data, uint8_t n);

int8_t I2C_rw(const uint8_t adresse, uint8_t *dataOut, uint8_t nOut, uint8_t *dataIn, uint8_t nIn);

#endif
