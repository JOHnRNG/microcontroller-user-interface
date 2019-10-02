/* Define and initialize an escape sequence for cursor position */
#include "VT100.h"

struct CUP cup = { ESC, '[', '0', '1', ';', '0', '1', 'H', NUL };

void update_cup(int dline, int dcol)
{
    int line, col;

    line = atoi(cup.line);
    col = atoi(cup.col);

    if ((dcol > 0) && (((col + dcol) > MAX_COL))) //right shift, exceed col range
    {
        col = (col + dcol) % MAX_COL;
        line = (line + 1) % MAX_LINE; //down one line

    }
    else if ((dcol > 0) && (((col + dcol) <= MAX_COL)))
        col += dcol;
    else if ((dcol < 0) && (((col + dcol) <= MIN_COL)))
        col = 1;
    else if ((dcol < 0) && (((col + dcol) > MIN_COL)))
        col -= 1;

    if ((dline > 0) && (((line + dline) <= MAX_LINE)))
        line += dline;
    else if ((dline > 0) && (((line + dline) > MAX_LINE)))
    {
        //line = (line + dline) % MAX_LINE; //down one line
        send_char(ESC);
        send_char('E');
        send_char(NUL);
    }

    cup.col[0] = INT_TO_CHAR((col % 100) / 10);
    cup.col[1] = INT_TO_CHAR(col % 10);
    cup.line[0] = INT_TO_CHAR((line % 100) / 10);
    cup.line[1] = INT_TO_CHAR(line % 10);

}

/*************************************************************************
 send a char to a string
 *************************************************************************/

void send_char(char in_data)
{
    //if (UART0_FR_R & UART_FR_TXFE) // UART transmit empty
    //{
    //    UART0_DR_R = in_data;
    //}
    //else
    //{
        enqueue(UART_OUT, in_data); //send the string
    //}
}

void send_str(char * str)
{
    int i = 0;
    int length = strlen(str);

    while (i < length)
    {
        send_char(str[i]);
        i += 1;
    }
}

void send_cup(void)
{
    send_char(cup.esc);
    send_char(cup.sqrbrkt);
    send_char(cup.line[0]);
    send_char(cup.line[1]);
    send_char(cup.semicolon);
    send_char(cup.col[0]);
    send_char(cup.col[1]);
    send_char(cup.cmdchar);
    send_char(cup.nul);
}

