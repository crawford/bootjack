/*
 * Copyright 2013 Alex Crawford
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "avr109.h"
#include "serial.h"

#define BUFFER_SIZE    SPM_PAGESIZE

static inline uint32_t read_flash_page(uint32_t addr, uint16_t size);
static inline uint32_t write_flash_page(uint32_t addr, uint16_t size);
static inline uint16_t read_eeprom_page(uint16_t addr, uint16_t size);
static inline uint16_t write_eeprom_page(uint16_t addr, uint16_t size);
static inline void led_on(void);
static inline void led_off(void);
static inline void erase_flash(void);

bool avr109_process(void)
{
	char command;
	uint32_t address = 0;

	while (true)
	{
		command = serial_get_char();

		if (command == 'P')        // Entering programming mode
		{
		}
		if (command == 'L')        // Leaving programming mode
		{
		}
		else if (command == 'T')   // Select device type
		{
			// Discard the device selected
			serial_get_char();
		}
		else if (command == 'E')   // Exit bootloader
		{
			break;
		}
		else if (command == 'a')   // Auto increment address
		{
			serial_send_char('Y');
			continue;
		}
		else if (command == 'A')   // Set word address
		{
			// Read a word address but store a byte address
			address = (((uint32_t)serial_get_char() << 8) |
			            (uint32_t)serial_get_char() << 0) << 1;
		}
		else if (command == 'b')   // Check block support
		{
			serial_send_char('Y');
			serial_send_char(BUFFER_SIZE >> 8);
			serial_send_char(BUFFER_SIZE & 0xFF);
			continue;
		}
		else if (command == 'B')   // Start block load
		{
			uint16_t size = ((uint16_t)serial_get_char() << 8) |
			                 (uint16_t)serial_get_char() << 0;
			char type = serial_get_char();
			if (type == 'F')       // Flash
				address= write_flash_page(address, size);
			else if (type == 'E')  // EEPROM
				address= write_eeprom_page(address, size);
		}
		else if (command == 'g')   // Start block read
		{
			uint16_t size = ((uint16_t)serial_get_char() << 8) |
			                 (uint16_t)serial_get_char() << 0;
			char type = serial_get_char();
			if (type == 'F')       // Flash
				address= read_flash_page(address, size);
			else if (type == 'E')  // EEPROM
				address= read_eeprom_page(address, size);
			continue;
		}
#ifdef CONFIG_AVR109_WRITE_LOCK
		else if (command == 'l')   // Write lock bits
		{
			boot_lock_bits_set(serial_get_char());
		}
#endif // CONFIG_AVR109_WRITE_LOCK
		else if (command == 'r')   // Read lock bits
		{
			serial_send_char(boot_lock_fuse_bits_get(GET_LOCK_BITS));
			continue;
		}
		else if (command == 'F')   // Read low fuse bits
		{
			serial_send_char(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
			continue;
		}
		else if (command == 'N')   // Read high fuse bits
		{
			serial_send_char(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
			continue;
		}
		else if (command == 'Q')   // Read extended fuse bits
		{
			serial_send_char(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));
			continue;
		}
#ifdef CONFIG_AVR109_CHIP_ERASE
		else if (command == 'e')   // Chip erase
		{
			erase_flash();
		}
#endif // CONFIG_AVR109_CHIP_ERASE
		else if (command == 't')   // List available device types
		{
			serial_send_char(DEVTYPE_BOOT);
			serial_send_char(0);
			continue;
		}
		else if (command == 'x')   // Enable LED
		{
			// Discard the LED option
			serial_get_char();
			led_on();
		}
		else if (command == 'y')   // Disable LED
		{
			// Discard the LED option
			serial_get_char();
			led_off();
		}
		else if (command == 'S')   // Get software identifier
		{
			serial_send_str("AVRBOOT");
			continue;
		}
		else if (command == 'V')   // Get software version
		{
			serial_send_char(((VERSION_CODE >> 8) & 0xFF) + '0');
			serial_send_char(((VERSION_CODE >> 0) & 0xFF) + '0');
			continue;
		}
		else if (command == 'p')   // Get programmer type
		{
			// Return 'S' for serial
			serial_send_char('S');
			continue;
		}
		else if (command == 's')   // Get device signature
		{
			serial_send_char(SIGNATURE_2);
			serial_send_char(SIGNATURE_1);
			serial_send_char(SIGNATURE_0);
			continue;
		}
		else                       // Unknown command
		{
			serial_send_char('?');
			continue;
		}
		serial_send_char('\r');
	}

	serial_send_char('\r');
	return false;
}

static inline uint32_t read_flash_page(uint32_t addr, uint16_t size)
{
	uint16_t i;
	for (i = size; i > 0; addr += 2, i -= 2)
	{
		uint16_t data;
#ifndef CONFIG_AVR109_READ_BOOT
		if (addr >= CONFIG_BOOT_START_ADDR)
		{
			data = 0xFFFF;
		}
		else
#endif // CONFIG_AVR109_READ_BOOT
		{
			data = pgm_read_word_far(addr);
		}

		serial_send_char(data);
		serial_send_char(data >> 8);
	}

	return addr;
}

static inline uint32_t write_flash_page(uint32_t addr, uint16_t size)
{
	uint32_t pagestart = addr;
	uint8_t sreg = SREG;
	uint8_t buffer[BUFFER_SIZE];
	uint8_t *p_buf;
	uint16_t i;

	for (i = size, p_buf = buffer; i > 0; i--)
		*p_buf++ = serial_get_char();

	cli();

	for (i = size, p_buf = buffer; i > 0; addr += 2, i -= 2, p_buf += 2)
	{
		uint16_t data = (*(p_buf + 1) << 8) | *p_buf;
		boot_page_fill(addr, data);
	}

	boot_page_write(pagestart);
	boot_spm_busy_wait();
	boot_rww_enable();

	SREG = sreg;
	return addr;
}

static inline uint16_t read_eeprom_page(uint16_t addr, uint16_t size)
{
	for (; size; addr++, size--)
		serial_send_char(eeprom_read_byte((uint8_t *)addr));

	eeprom_busy_wait();

	return addr;
}

static inline uint16_t write_eeprom_page(uint16_t addr, uint16_t size)
{
	for (; size; addr++, size--)
		eeprom_write_byte((uint8_t *)addr, serial_get_char());

	eeprom_busy_wait();

	return addr;
}

static inline void led_on(void)
{

}

static inline void led_off(void)
{

}

static inline void erase_flash(void)
{
	uint32_t addr = 0;
	while (addr < CONFIG_BOOT_START_ADDR)
	{
		boot_page_erase(addr);
		boot_spm_busy_wait();
		addr += SPM_PAGESIZE;
	}
	boot_rww_enable();
}

