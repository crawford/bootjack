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


#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "avr109.h"
#include "vt100.h"
#include "serial.h"
#include "menu.h"

static inline void mcu_init(void);
static inline void mcu_kill(void);
static inline bool app_present(void);

/*
 * Submenu items
 */

static const struct menu_item menu_sub1 = {
	.description = "Submenu Item 1",
};

static const struct menu_item menu_sub2 = {
	.description = "Submenu Item 2",
};

static const struct menu_item *sub_menu_items[] = { &menu_sub1,
                                                    &menu_sub2,
                                                    0 };

/*
 * Main menu items
 */

static inline bool start_bootloader(void)
{
	serial_send_str(VT100_CLEAR_SCREEN
	                VT100_MOVE_CURSOR_HOME
	                "Running avr109 bootloader. Connect programmer...");
	return avr109_process();
}

static const struct menu_item menu_bootloader = {
	.description = "AVR109 Bootloader",
	.action = &start_bootloader,
};

static const struct menu_item menu_test = {
	.description = "Example Submenu",
	.sub_menu = sub_menu_items
};

static const struct menu_item *main_menu_items[] = { &menu_bootloader,
                                                     &menu_test,
                                                     0 };

/*
 * Main menu
 */

static const struct menu main_menu = {
	.title = "Example Menu",
	.items = main_menu_items
};


void __attribute__((noreturn)) exit(int code)
{
	(void)code;

	__asm("jmp %0" : : "i"(CONFIG_APP_START_ADDR));
	__builtin_unreachable();
}

int main(void)
{
	mcu_init();
	serial_init();

	while (!app_present())
		menu_exec(&main_menu);

	serial_kill();
	mcu_kill();
}

static inline void mcu_init(void)
{
#ifdef CONFIG_USE_INTERRUPTS
	// Move the IVT to the start of the boot loader section
	uint8_t t_mcucr = MCUCR;
	MCUCR = t_mcucr | (1 << IVCE);
	MCUCR = t_mcucr | (1 << IVSEL);
	// Enable global interrupts
	SREG = (1 << SREG_I);
#endif // CONFIG_USE_INTERRUPTS
}

static inline void mcu_kill(void)
{
#ifdef CONFIG_USE_INTERRUPTS
	// Move the IVT to the start of flash
	uint8_t t_mcucr = MCUCR;
	MCUCR = t_mcucr | (1 << IVCE);
	MCUCR = t_mcucr & ~(1 << IVSEL);
	// Disable global interrupts
	SREG = (0 << SREG_I);
#endif // CONFIG_USE_INTERRUPTS
}

static inline bool app_present(void)
{
	const __flash volatile uint16_t *start = CONFIG_APP_START_ADDR;
	return (*start != 0xFFFF);
}

