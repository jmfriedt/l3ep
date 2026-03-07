#include "libVirtualSerial.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{.Config =
			{.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
			 .DataINEndpoint           =
					{.Address          = CDC_TX_EPADDR,
					 .Size             = CDC_TXRX_EPSIZE,
					 .Banks            = 1,
					},
				.DataOUTEndpoint =
					{.Address          = CDC_RX_EPADDR,
					 .Size             = CDC_TXRX_EPSIZE,
					 .Banks            = 1,
					},
				.NotificationEndpoint =
					{.Address          = CDC_NOTIFICATION_EPADDR,
					 .Size             = CDC_NOTIFICATION_EPSIZE,
					 .Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */
FILE USBSerialStream;

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	USB_Init();
}

/** Event handler for the library USB Connection event. */
//void EVENT_USB_Device_Connect(void)
//{
//	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
//}

/** Event handler for the library USB Disconnection event. */
//void EVENT_USB_Device_Disconnect(void)
//{
//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
//}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}
