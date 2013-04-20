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


#ifndef __SERIAL_H__
#define __SERIAL_H__

#define UBRR           (((F_CPU) / 16 / (CONFIG_BAUD_RATE)) - 1)

void serial_init(void);
void serial_kill(void);
void serial_send_char(unsigned char c);
void serial_send_str(const char *str);
void serial_send_int(int num);
void serial_send_alphanum(int num);
unsigned char serial_get_char(void);
int serial_get_alphanum(void);

#endif // __SERIAL_H__

