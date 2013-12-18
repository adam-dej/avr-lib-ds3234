/*
Copyright 2013 Adam Dej.
This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DS3234_H
#define __DS3234_H

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

#define DS3234_REG_CONTROL 0x0E
#define DS3234_REG_CTRL_STATUS 0x0F
#define DS3234_REG_CRYSTAL_AGING 0x10
#define DS3234_REG_VBAT_TMP_DISABLE 0x13

/**
 * Initializes SPI for use with the DS3234.
 * The paramteres are 4 SPI-related functions, which due to the portability
 * user must write himself.
 * 
 * A pointer to the SPI init function. Can be set to the custom function.
 * This function must initialize the SPI bus in Master mode, set the slave select pin to which
 * _CS of the DS3234 is connected to the desired mode and drive it high.
 * It should also select and unselect the chip once, otherwise the if the
 * first operation with the chip is writing time register the write won't work. See DS3234's datasheet
 * for the SPI parameters.
 * 
 * A pointer to the SPI transfer function. Can be set to the custom function.
 * This function must send a byte over the SPI bus to which DS3234 is connected and
 * return the received byte. This function MUST NOT change state of the slave select pin.
 *
 * A pointer to the SPI slave select function. Can be set to the custom function.
 * This function must drive the pin to which DS3234's _CS is connected low
 *
 * A pointer to the SPI slave unselect function. Can be set to the custom function.
 * This function must drive the pin to which DS3234's _CS is connected high
 */
void ds3234_init(void (*spi_ds3234_init)(), uint8_t (*spi_ds3234_transfer)(uint8_t), 
	void (*spi_ds3234_slave_select)(), void (*spi_ds3234_slave_unselect)());

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
 * control holds a century bit (bit 0). If bit 1 is set only one of day_of_week
 * or day_of_month values are valid, and in that case bit 2 denotes which one
 * (0: day_of_week, 1: day_of_month)
 */
typedef struct {
	uint8_t day_of_week;
	uint8_t day_of_month;
	uint8_t month;
	uint8_t year;
	uint8_t control;
} DS3234_DATE;

/**
 * A function for reading time from DS3234's main time register.
 */
void ds3234_read_time(DS3234_TIME *time);

/**
 * A function for writing time to DS3234's main time register.
 * Please note that this function does not do any error checking, so
 * please make sure the values are valid and in range.
 */
void ds3234_write_time(DS3234_TIME *time);

/**
 * A function for reading date from DS3234's main date register.
 * In returned DS3234_DATE structure both day_of_week and day_of_month are valid.
 */
void ds3234_read_date(DS3234_DATE *date);

/**
 * A function for writing date to DS3234's main date register.
 * Please note that this function does not do any error checking. Both day_of_week
 * and day_of_month must be valid.
 */
void ds3234_write_date(DS3234_DATE *date);

/**
 * A function for reading temperature registers of DS3234.
 * See DS3234's datasheet page 16 for the format description.
 */
int16_t ds3234_read_temp();

/**
 * A function for triggering temperature conversion
 */
void ds3234_trigger_conversion();

/**
 * A function for reading a single register.
 */
uint8_t ds3234_read_register(uint8_t address);

/**
 * A function for writing a single register.
 * You may also use the read address, as this is converted to write address if necessary.
 */
void ds3234_write_register(uint8_t address, uint8_t data);

/**
 * A function for reading alarm 1 from DS3234.
 */
void ds3234_read_alarm1(DS3234_DATE *date, DS3234_TIME *time, uint8_t *alarm_mask);

/**
 * A function for writing Alarm 1 to DS3234.
 * Please note that this won't actualy enable the alarm, for that you need to modify
 * the control register. See DS3234's datasheet for more details.
 */
void ds3234_write_alarm1(DS3234_DATE *date, DS3234_TIME *time, uint8_t alarm_mask);

/**
 * A function for reading alarm 2 from DS3234.
 */
void ds3234_read_alarm2(DS3234_DATE *date, DS3234_TIME *time, uint8_t *alarm_mask);

/**
 * A function for writing Alarm 2 to DS3234.
 * Please note that this won't actualy enable the alarm, for that you need to modify
 * the control register. See DS3234's datasheet for more details.
 */
void ds3234_write_alarm2(DS3234_DATE *date, DS3234_TIME *time, uint8_t alarm_mask);

/**
 * A function for reading RAM of DS3234.
 * Make sure the *data is big enough for length of uint8_t's. If address + length > 256
 * the address will overflow and data from beginning will be read.
 */
void ds3234_read_RAM(uint8_t address, uint8_t length, uint8_t *data);

/**
 * A function for writing RAM of DS3234.
 * Make sure the *data is big enough for length of uint8_t's. If address + length > 256
 * the address will overflow and data will be written to the beginning of the RAM.
 */
void ds3234_write_RAM(uint8_t address, uint8_t length, uint8_t *data);

#endif