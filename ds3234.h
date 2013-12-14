#ifndef __DS3234_H
#define __DS3234_H

#ifndef DD_MOSI
	#define DD_MOSI 3
#endif

#ifndef DD_MISO
	#define DD_MISO 4
#endif

#ifndef DD_SCK
	#define DD_SCK 5
#endif

#ifndef DD_SS
	#define DD_SS 2
#endif

#ifndef DDR_SPI
	#define DDR_SPI DDRB
#endif

#ifndef DDR_DS3234_SS
	#define DDR_DS3234_SS DDRB
#endif

#ifndef PORT_DS3234_SS
	#define PORT_DS3234_SS PORTB
#endif

#ifndef DS_3234_SS
	#define DS_3234_SS 2
#endif

/**
 * A default function for SPI bus initialization in the Master mode
 */
void __ds3234_spi_init() {
	DDR_SPI |= (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_SS);
	DDR_SPI &= ~(1<<DD_MISO);

	DDR_DS3234_SS |= (1<<DS_3234_SS);

	SPCR = (1<<MSTR) | (1<<CPOL) | (1<<CPHA);
	SPCR |= (1<<SPE);
}

/**
  * A default function for a single SPI transfer (blocking)
  */
uint8_t __ds3234_spi_transfer(uint8_t data) {
	SPDR = data;
	while(!(SPSR & (1 <<SPIF)));
	return SPDR;
}

/**
 * A default function for slave select of DS3234
 */
void __ds3234_spi_slave_select() {
	PORT_DS3234_SS &= ~(1<<DS_3234_SS);
}

/**
 * A default function for unselecting DS3234
 */
void __ds3234_spi_slave_unselect() {
	PORT_DS3234_SS |= (1<<DS_3234_SS);
}

/**
 * A pointer to the SPI init function. Can be set to the custom function.
 * This function must initialize the SPI bus in Master mode, set the slave select pin to which
 * _CS of the DS3234 is connected to the desired mode and drive it high. See DS3234's datasheet
 * for the SPI parameters.
 * @see __ds3234_spi_init()
 */
void (*_ds3234_init)() = __ds3234_spi_init;

/**
 * A pointer to the SPI transfer function. Can be set to the custom function.
 * This function must send a byte over the SPI bus to which DS3234 is connected and
 * return the received byte. This function MUST NOT change state of the slave select pin.
 */
uint8_t (*_ds3234_transfer)(uint8_t) = __ds3234_spi_transfer;

/**
 * A pointer to the SPI slave select function. Can be set to the custom function.
 * This function must drive the pin to which DS3234's _CS is connected low
 */
void (*_ds3234_slave_select)() = __ds3234_spi_slave_select;

/**
 * A pointer to the SPI slave unselect function. Can be set to the custom function.
 * This function must drive the pin to which DS3234's _CS is connected high
 */
void (*_ds3234_slave_unselect)() = __ds3234_spi_slave_unselect;

/**
 * A structure holding time in a format with which the DS3234 is working.
 * ampm_mask holds information about wheter the clock is in 12/24 mode (bit 0)
 * and if in 12 mode bit 1 denotes AM/PM mode (0: AM, 1: PM)
 */
typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t ampm_mask;
} DS3234_TIME;

/**
 * A structure holding time in a format with which the DS3234 is working.
 * control holds a century bit (bit 1) and an information whether the day_of_week (0)
 * or day_of_month (1) is valid (bit 0) where applicable (reading and writing alarm registers)
 */
typedef struct {
	uint8_t day_of_week;
	uint8_t day_of_month;
	uint8_t month;
	uint8_t year;
	uint8_t control;
} DS3234_DATE;




#endif