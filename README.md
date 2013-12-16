# lib-ds3234

A library for interfacing the DS3234 (SPI) RTC to an AVR microcontroller.

## Usage

If you wish to use builtin SPI functions `#define DS3234_USE_DEFAULT_SPI` before you include the library.

If you wish to use custom functions write them, see comments for function pointers in the library to find out what they should do, and assign those function pointers to your functions **before** calling `ds3234_init()`.

Please consult the DS3234s datasheet when using this library, many things are explained there.

## Examples

### Reading time stored in the DS3234

    ds3234_init();

    DS3234_TIME *time = malloc(sizeof(DS3234_TIME));
    ds3234_read_time(time);

### Reading 2 bytes of DS3234's RAM

    ds3234_init();
    
    uint8_t data[2];
    ds3234_read_RAM(0, 2, data);

### Disabling the 32KHz square output (changing the Control/Status register, see DS3234s datasheet for details)

    ds3234_init();
    
    uint8_t constat_register; //Short for control-status register
    constat_register = ds3234_read_register(DS3234_REG_CTRL_STATUS);
    constat_register &= ~(1 << DS3234_CONSTAT_EN32KHZ);
    ds3234_write_register(DS3234_REG_CTRL_STATUS, constat_register);
