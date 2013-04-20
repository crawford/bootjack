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


#include <avr/io.h>
#include "serial.h"

void serial_send_char(unsigned char c)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

void serial_send_str(const char *str)
{
	for (; *str; str++)
		serial_send_char(*str);
}

void serial_send_int(int num)
{
	int pow;

	if (num < 0) {
		serial_send_char('-');
		num *= -1;
	}

	for (pow = 1; pow * 10 <= num; pow *= 10);

	for (; pow > 0; pow /= 10)
		serial_send_char('0' + (num / pow) % 10);
}

void serial_send_alphanum(int num)
{
	if (num >= 0 && num < 10)
		serial_send_char('0' + num);
	else if (num >= 10 && num < 36)
		serial_send_char('A' + num - 10);
}

void serial_init(void)
{
	UBRR0H = (UBRR >> 8) & 0xFF;
	UBRR0L = UBRR & 0xFF;

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B = (1 << RXEN0)  |
	         (1 << TXEN0);
}

void serial_kill(void)
{
	UCSR0B = (0 << RXEN0) |
	         (0 << TXEN0);
}

unsigned char serial_get_char(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

int serial_get_alphanum(void)
{
	char c = serial_get_char();
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'Z')
		return (c - 'A');

	return -1;
}

