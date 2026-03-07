## libLEEA

Compile from the ``src`` directory to generate the library
stored in ``lib``

## Function description

The I2C communication library includes
* ``void I2C_UEXT(uint8_t on);``: PB4 high to power the supply voltage (1 for on, 0 for off)
* ``void I2C_init(uint8_t bitrate, uint8_t prescaler);``: initialize I2C communication clock (bitrate=0x18 for 400 kHz)
* ``int8_t I2C_write(const uint8_t  address, uint8_t *data, uint8_t n);``: write ``n`` bytes from ``data`` to
``address`` slave
* ``int8_t I2C_read(const uint8_t address, uint8_t *data, uint8_t n);``: read ``n`` bytes to ``data`` from
``address`` slave
* ``int8_t I2C_rw(const uint8_t address, uint8_t *dataOut, uint8_t nOut, uint8_t *dataIn, uint8_t nIn);``: write
``nOut`` bytes and read ``nIn`` bytes to/from ``address``.

ADC initialization functions:
* ``void adc_init(uint8_t)``: called once for initializing the ADC speed and setting the reference voltage with a mask [REFS1,REFS0] provided as argument (0 for AREF, 1 for Vcc, 3 for internal
2.56V reference).
* ``unsigned short adc_read(uint8_t)``: called for reading a new measurement on a channel given as argument (0..31).
