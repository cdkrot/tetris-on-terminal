// terminal control functions.
#ifndef __TERMINAL_H

#define __TERMINAL_H

#include <string>
#include "./util/char_data.h"
#include "./stdstyle.h"

void terminal_init();
void terminal_reset();
void terminal_put_char(char ch, color_t col);
void terminal_put_char(char c);
void terminal_put_string(std::string s);
void terminal_put_string(std::string str, color_t col);
void terminal_endline();
void terminal_flush();
void terminal_shutdown();

#endif