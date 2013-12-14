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


//Please consult DS3234's datasheet on meaning and proper usage of these constants
#define DS3234_ALARM1_ONCE_PER_SECOND 0x0F
#define DS3234_ALARM1_SECONDS_MATCH 0x0E
#define DS3234_ALARM1_MINUTES_SECONDS_MATCH 0x0C
#define DS3234_ALARM1_HOURS_MINUTES_SECONDS_MATCH 0x08
#define DS3234_ALARM1_DATE_HOURS_MINUTES_SECONDS_MATCH 0x00
#define DS3234_ALARM1_DAY_HOURS_MINUTES_SECONDS_MATCH 0x10

#define DS3234_ALARM2_ONCE_PER_MINUTE 0x07
#define DS3234_ALARM2_MINUTES_MATCH 0x06
#define DS3234_ALARM2_HOURS_MINUTES_MATCH 0x04
#define DS3234_ALARM2_DATE_HOUR_MINUTES_MATCH 0x00
#define DS3234_ALARM2_DAY_HOURS_MINUTES_MATCH 0x08

#define DS3234_CONTROL_EOSC 7
#define DS3234_CONTROL_BBSQW 6
#define DS3234_CONTROL_CONV 5
#define DS3234_CONTROL_RS2 4
#define DS3234_CONTROL_RS1 3
#define DS3234_CONTROL_INTCN 2
#define DS3234_CONTROL_A2IE 1
#define DS3234_CONTROL_A1IE 0

#define DS3234_CONSTAT_OSF 7
#define DS3234_CONSTAT_BB32KHZ 6
#define DS3234_CONSTAT_CRATE1 5
#define DS3234_CONSTAT_CRATE0 4
#define DS3234_CONSTAT_EN32KHZ 3
#define DS3234_CONSTAT_BSY 2
#define DS3234_CONSTAT_A2F 1
#define DS3234_CONSTAT_A1F 0

#define BCD_TO_INT(X) (X & 0x0F) + (10*(X >> 4))
#define INT_TO_BCD(X) (X % 10) | ((X / 10) << 4)

/**
 * A default function for SPI bus initialization in the Master mode
 */
void __ds3234_spi_init() {
	DDR_SPI |= (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_SS);
	DDR_SPI &= ~(1<<DD_MISO);

	DDR_DS3234_SS |= (1<<DS_3234_SS);

	SPCR = (1<<MSTR) | (1<<CPOL) | (1<<CPHA);
	SPCR |= (1<<SPE);

	PORT_DS3234_SS &= ~(1<<DS_3234_SS);
	PORT_DS3234_SS |= (1<<DS_3234_SS);
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
 * _CS of the DS3234 is connected to the desired mode and drive it high.
 * It should also select and unselect the chip once, otherwise the if the
 * first operation with the chip is writing time register the write won't work. See DS3234's datasheet
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
 * ampm_mask holds information about whether the clock is in 12/24 mode (bit 0)
 * (1 if 12 mode, 0 if 24 mode)
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

/**
 * A function for reading time from DS3234's main time register
 * This function is using burst reading. See DS3234's datasheet.
 */
void ds3234_read_time(DS3234_TIME *time) {
	_ds3234_slave_select();
	_ds3234_transfer(0x00); //Seconds register
	uint8_t data = _ds3234_transfer(0x00); //Dummy byte to receive seconds register
	time->seconds = BCD_TO_INT(data);
	data = _ds3234_transfer(0x00); //Dummy byte to burst receive minutes register
	time->minutes = BCD_TO_INT(data);
	data = _ds3234_transfer(0x00); //Dummy byte to burst reveive hours register
	if (data & (1 << 6)) {
		//Register is in 12 hour mode
		//If bit 4 is set (10hr bit) increment hour by 10 (Captain Obvious!)
		time->hours = (data & (1 << 4)) ? (data & 0x0F) + 10 : (data & 0x0F);
		time->ampm_mask = 1;
		time->ampm_mask |= (data & (1 << 5) >> 4); //bit 5 (AM/PM) of date shifted to bit 1 of ampm_mask
	} else {
		//Register is in 24 hour mode
		//Bit 6 and 7 are guaranteed to be zero in this mode
		time->hours = BCD_TO_INT(data);
		time->ampm_mask = 0;
	}
	_ds3234_slave_unselect();
}

/**
 * A function from writing time to DS3234's main time register
 * This function is using burst writing. See DS3234's datasheet.
 * Please note that this function does not do any error checking, so
 * please make sure the values are valid and in range.
 */
void ds3234_write_time(DS3234_TIME *time) {
	_ds3234_slave_select();
	_ds3234_transfer(0x80); //Write seconds register address
	_ds3234_transfer(INT_TO_BCD(time->seconds)); //Write seconds register
	_ds3234_transfer(INT_TO_BCD(time->minutes)); //Burst-write minutes register
	if (time->ampm_mask) {
		//The clock is in 12-hour mode;
		_ds3234_transfer(INT_TO_BCD(time->hours) | (1 << 6) | ((time->ampm_mask & 2) << 5));
		//Burst-write hours register, bit 6 meaning we are in 12 hours mode and bit 5 AM/PM value
	} else {
		//The clock is in 24-hour mode
		_ds3234_transfer(INT_TO_BCD(time->hours)); //Burst-write hours register
	}
	_ds3234_slave_unselect();
}


#endif