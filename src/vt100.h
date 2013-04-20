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


#ifndef __VT_100__
#define __VT_100__

#ifdef CONFIG_USE_VT100

#define VT100_ATTR_CLEAR           "\x1B[0m"
#define VT100_ATTR_BOLD            "\x1B[1m"
#define VT100_ATTR_LOW_INTENSITY   "\x1B[2m"
#define VT100_ATTR_UNDERLINE       "\x1B[4m"
#define VT100_ATTR_BLINK           "\x1B[5m"

#define VT100_CLEAR_SCREEN         "\x1B[2J"

#define VT100_MOVE_CURSOR_HOME     "\x1B[H"

#else // CONFIG_USE_VT100

#define VT100_ATTR_CLEAR           ""
#define VT100_ATTR_BOLD            ""
#define VT100_ATTR_LOW_INTENSITY   ""
#define VT100_ATTR_UNDERLINE       ""
#define VT100_ATTR_BLINK           ""

#define VT100_CLEAR_SCREEN         ""

#define VT100_MOVE_CURSOR_HOME     ""

#endif // CONFIG_USE_VT100

#endif // __VT_100__

