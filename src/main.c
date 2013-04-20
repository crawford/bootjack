#include <stdbool.h>
#include <avr/interrupt.h>
#include "avr109.h"
#include "vt100.h"
#include "serial.h"
#include "menu.h"
#include "examplemenu.h"

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
	const __flash volatile uint16_t *start = CONFIG_APP_START_ADDR;
	return (*start != 0xFFFF);
}

