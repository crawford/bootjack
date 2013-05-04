#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/version.h>
#include "avr109.h"
#include "vt100.h"
#include "serial.h"
#include "menu.h"
#include "examplemenu.h"

#define GCC_VERSION    (__GNUC__ * 10000 +     \
                        __GNUC_MINOR__ * 100 + \
                        __GNUC_PATCHLEVEL__)
#define GCC_4_7_1      (40701)
#define LIBC_1_8_0     (10800)

static inline void mcu_init(void);
static inline void mcu_kill(void);
static inline bool app_present(void);

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
#if (GCC_VERSION < GCC_4_7_1) && (__AVR_LIBC_VERSION__ >= LIBC_1_8_0)
	uint16_t start = pgm_read_word_far(CONFIG_APP_START_ADDR);
#else
	uint16_t start = *(const __flash uint16_t *)CONFIG_APP_START_ADDR;
#endif
	return (start != 0xFFFF);
}

