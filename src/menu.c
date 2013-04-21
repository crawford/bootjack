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


#include <stdlib.h>
#include "serial.h"
#include "vt100.h"
#include "menu.h"

static bool menu_items_exec_choice(const char *title, const struct menu_item **items);
static inline const struct menu_item * menu_items_get_choice(const char *title, const struct menu_item **items);
static inline int menu_draw(const char *title, const struct menu_item **items);

void menu_exec(const struct menu *menu)
{
	menu_items_exec_choice(menu->title, menu->items);
}

static bool menu_items_exec_choice(const char *title, const struct menu_item **items)
{
	const struct menu_item *sel;

	while (true)
	{
		sel = menu_items_get_choice(title, items);

		// If EXIT was chosen
		if (sel == NULL)
			return true;

		if (sel->action)
			if (!sel->action())
				return false;

		if (sel->sub_menu)
			if (!menu_items_exec_choice(sel->description, sel->sub_menu))
				return false;
	}
}

static inline const struct menu_item * menu_items_get_choice(const char *title, const struct menu_item **items)
{
	int sel = 0;
	int count = menu_draw(title, items);

	while (true)
	{
		serial_send_str("Selection: ");
		sel = serial_get_alphanum();

		if (sel >= 0 && sel < count)
			break;

		serial_send_str("Invalid Selection\n\r");
	}

	return items[sel];
}

static inline int menu_draw(const char *title, const struct menu_item **items)
{
	int index;

	serial_send_str(VT100_CLEAR_SCREEN
	                VT100_MOVE_CURSOR_HOME
	                VT100_ATTR_BOLD
	                VT100_ATTR_UNDERLINE);
	serial_send_str(title);
	serial_send_str(VT100_ATTR_CLEAR "\n\r");

	for (index = 0; *items; items++, index++)
	{
		serial_send_str("  ");
		serial_send_alphanum(index);
		serial_send_str(" - ");
		serial_send_str((*items)->description);
		serial_send_str("\n\r");
	}
	serial_send_str("  ");
	serial_send_alphanum(index);
	serial_send_str(" - Exit\n\r");

	return index + 1;
}

