#ifndef __USBAPI__
#define __USBAPI__

#include "Platform.h"
//================================================================================
//================================================================================
//	USB
	u8 USBDevice_configured(void);

	void  USBDevice_attach(void);

//================================================================================
//================================================================================



	void Serial_begin(uint16_t baud_count);
	void Serial_end(void);

	int Serial_available(void);
	void Serial_accept(void);
        int Serial_peek(void);
	int Serial_read(void);
	void Serial_flush(void);
	size_t Serial_write(uint8_t);
	u8 Serial_test(void);



//================================================================================
//================================================================================
//	Low level API

typedef struct
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t wValueL;
	uint8_t wValueH;
	uint16_t wIndex;
	uint16_t wLength;
} Setup;

//==============================================================================


#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

int USB_SendControl(uint8_t flags, const void* d, int len);
int USB_RecvControl(void* d, int len);

uint8_t	USB_Available(uint8_t ep);
int USB_Send(uint8_t ep, const void* data, int len);	// blocking
int USB_Recv(uint8_t ep, void* data, int len);		// non-blocking
int USB_RecvNB(uint8_t ep);							// non-blocking
void USB_Flush(uint8_t ep);

#endif


