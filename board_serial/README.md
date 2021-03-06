Serial port interrupt demonstration.

Uncomment PORT B toggling to generate VCD trace with serial port data and interrupt events.

Requires ``simavr``, available at https://github.com/buserror/simavr

1. clone simavr
2. to make visualization of the GPIO status easier, add to ``avr_ioport_write()`` in 
``simavr/sim/avr_ioport.c`` the following display function

```
static void avr_ioport_write(struct avr_t * avr, avr_io_addr_t addr, uint8_t v, void * param)
{printf("\nSIMAVR: IOPORT @0x%x<-0x%x\n",addr,v);fflush(stdout);
 avr_ioport_t * p = (avr_ioport_t *)param;
 [...]
```

3. copy this directory in the examples subdirectory of ``simavr``
4. install ``libelf-dev`` if not available (required dependency)
5. in simavr: ``make``
will recursively compile all board examples including the content of this directory.
6. execute ``./examples/board_project/obj-x86_64-linux-gnu/serial.elf``

