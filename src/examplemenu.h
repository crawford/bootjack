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


#ifndef __EXAMPLE_MENU_H__
#define __EXAMPLE_MENU_H__

static inline bool start_bootloader(void);

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



static inline bool start_bootloader(void)
{
	serial_send_str(VT100_CLEAR_SCREEN
	                VT100_MOVE_CURSOR_HOME
	                "Running avr109 bootloader. Connect programmer...");
	return avr109_process();
}

#endif // __EXAMPLE_MENU_H__

