/*
 * corps de la bibliothèque I2C
*/
/******************************************************************************/
#include <avr/io.h>
#include "libi2c.h"
#include "libttycom.h"

// UEXT_PWR_E PB4

#ifdef DEBUG
char debug[80]="";
#endif

void I2C_UEXT(uint8_t on){
  DDRB|=1<<PB4;
  if(on)
    PORTB&=~1<<PB4;
  else
    PORTB|=1<<PB4;
}

void I2C_init(uint8_t bitrate, uint8_t prescaler)
     //Séléction de la fréquence 
{
  PRR0&=~1<<PRTWI;
  // activate internal pullups for twi.
  /* digitalWrite(SDA, 1); */
  /* digitalWrite(SCL, 1); */
  TWBR = bitrate;
  TWSR = prescaler & 0x03;
  TWCR = 1<<TWEN ;
}

/******************************************************************************/
int8_t I2C_action(int8_t command){
//starts any I2C operation (not stop), waits for completion and returns the status code
  TWCR = (command|(1<<TWINT)|(1<<TWEN));
  while(!(TWCR & (1<<TWINT)));
  //retour du code d'état
  return (TWSR & 0xF8);
}

/******************************************************************************/
int8_t I2C_start(void)
{	
  return (I2C_action(1<<TWSTA));
  //La valeur de retour est égale 0x08 (start) ou 0x10 (repeated start)
}

/******************************************************************************/
void I2C_stop(void)
// stop bit
{	

  TWCR = ((1<<TWINT)|(1<<TWSTO)|(1<<TWEN));
  /* while(TWCR & 1<<TWSTO){ */
  /* } */
}

/******************************************************************************/
int8_t I2C_write_data(uint8_t data)
{
  TWDR=data;
#ifdef DEBUG
  sprintf(debug ,"ligne  %d du fichier %s data:%x\n\r",__LINE__,__FILE__,data);
  USB_writestr(debug);
#endif
  return I2C_action(0);
  //La valeur de retour:
  //0x18 (slave ACKed address)
  //0x20 (no ACK after address)
  //0x28 (data ACKed by slave)
  //0x30 (no ACK after data transfer)
  //0x38 (lost arbitration)
}

/******************************************************************************/
int8_t I2C_read_data(int8_t put_ack)
{	//if an ACK is to returned to the transmitting device, set the TWEA bit
  if(put_ack)
    return(I2C_action(1<<TWEA));
  //si non ACK (a NACK) has to be returned, just receive the data
  else
    return(I2C_action(0));
  //La valeur de retour:
  //0x38 (lost arbitration)
  //0x40 (slave ACKed address)
  //0x48 (no ACK after slave address)
  //0x50 (AVR ACKed data)
  //0x58 (no ACK after data transfer)
}

/******************************************************************************/
int8_t I2C_write(const uint8_t address, uint8_t *data, uint8_t n)
{
  int8_t dummy;
  int i;
  //we need this for the first if()
  dummy = I2C_start();
  //if the start was successful, continue, otherwise return 1
  if((dummy != 0x08) && (dummy != 0x10)){
    I2C_stop();
    return dummy;
  }
  //now send the slave address
  //address format:
  if((dummy=I2C_write_data(address)) != 0x18){
    I2C_stop();
    return dummy;
  }
  for(i=0; i<n; i++){
    //now send the byte
    if((dummy=I2C_write_data(*data++)) != 0x28){
      I2C_stop();
      return dummy;
    }
  }
  //  if(restart==0)
  I2C_stop();
  //if everything was OK, return zero.
  return 0;
}

/******************************************************************************/
int8_t I2C_read(const uint8_t address,uint8_t *data, uint8_t n)
{
  int8_t dummy;
  int ack,i;
  //we need this for the first if()
  dummy = I2C_start();
  //as in_write_byte, first send the address 
  if((dummy != 0x08) && (dummy != 0x10)){
    I2C_stop();
    return dummy;
  }
  if((dummy=I2C_write_data(address|1)) != 0x40){
    I2C_stop();
    return dummy;
  }
  //now get the data , don't return ACK
  ack=1;
  for(i=0; i<n; i++){
    if (i==n-1) ack=0;
    if((dummy=I2C_read_data(ack)) != 0x50+(1-ack)*8){
      I2C_stop();
      return dummy;
    }
    *data++=TWDR; 
  }
  I2C_stop();
  //Si tout est OK, retourne zero.
  return 0;
}
/******************************************************************************/
int8_t I2C_rw(const uint8_t adresse, uint8_t *dataOut, uint8_t nOut, uint8_t *dataIn, uint8_t nIn)
{
  int err;
  // restart=1;
  if((err=I2C_write(adresse, dataOut, nOut))!=0){
    //restart=0;
    return err;}
  if((err=I2C_read(adresse, dataIn, nIn))!=0){
    //restart=0;
    return err;}  
  //restart=0;
  //Si tout est OK, retourne zero.
  return 0;
}

