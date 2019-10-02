/*
 * VT100.h
 *
 *  Created on: 2019Äê9ÔÂ25ÈÕ
 *      Author: Yu Gao
 */

#ifndef SRC_VT100_H_
#define SRC_VT100_H_


#define NUL 0x00
#define ESC 0x1b
#define ENTER 0x0d
#define TAB 0x09
#define BACKSPACE 127

#define MAX_LINE 24
#define MAX_COL 80
#define MIN_LINE 1
#define MIN_COL 1

#define INT_TO_CHAR(a) (a + 48)
#define CHAR_TO_INT(a) (a - 48)

#include <strings.h>
#include "Queue.h"

struct CUP
{
char esc;
char sqrbrkt;
char line[2];   /* 01 through 24 */
char semicolon;
char col[2];    /* 01 through 80 */
char cmdchar;
char nul;
};

extern struct CUP cup;

void update_cup(int delta_line, int delta_col);
void send_cup(void);
void send_char(char in_data);
void send_str(char * str);

#endif /* SRC_VT100_H_ */
