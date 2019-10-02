#include <stdio.h>
#include <string.h>
#include "Queue.h"
#include "Uart.h"
#include "CmdTbl.h"
#include "Systick.h"
#include "VT100.h"

#define UART_IN_SZ 50
#define UART_OUT_SZ 50
#define TIMER_SZ 50
#define MAX_STR_SZ 32

#define DECI_LOW_LIMIT 48
#define DECI_HIGH_LIMIT 57

#define MONTH_PER_YEAR 12

#define IsDecimal(a) ((a >= DECI_LOW_LIMIT) && (a <= DECI_HIGH_LIMIT))

#define LEAP_YEAR(y) (((y % 4 == 0) && (y % 100 != 0)) || ((y % 400 == 0) && (y % 3200 != 0)))

#define IS_HOUR(h) ((h < 24) && (h >= 0))
#define IS_MINUTE(m) ((m < 60) && (m >= 0))
#define IS_SEC(s) ((s < 60) && (s >= 0))
#define IS_TSEC(t) ((t <= 9) && (t >= 0))
#define IS_DOT(d) (d == 46)
#define IS_COLON(c) (c == 58)

typedef struct Days
{
    char month[4];
    int days;
} Days_t;

Days_t date_tbl[MONTH_PER_YEAR] =
        { { "JAN", 31 }, { "FEB", 28 }, { "MAR", 31 }, { "APR", 30 }, { "MAY",
                                                                        31 },
          { "JUN", 30 }, { "JUL", 31 }, { "AUG", 31 }, { "SEP", 30 }, { "OCT",
                                                                        31 },
          { "NOV", 30 }, { "DEC", 31 } };

int get_month(char * month)
{
    int i = 0;
    while (i < MONTH_PER_YEAR)
    {
        if (strcasecmp(date_tbl[i].month, month) == 0)
        {
            return (i + 1);
        }
        i += 1;
    }
    return -1;
}

typedef struct Time
{
    int hour;
    int min;
    int sec;
    int tsec;
} Time_t;

typedef struct Date
{
    int year;
    int month;
    int day;
} Date_t;

char time[11] = { '0', '0', ':', '0', '0', ':', '0', '0', '.', '0' };
Time_t decimal_time = { 0, 0, 0, 0 };

char date[12] = { 'J', 'A', 'N', '-', '0', '1', '-', '1', '9', '0', '0' };
Date_t decimal_date = { 1900, 1, 1 };

Time_t decimal_alarm = { 0, 0, 0, 0 };
Time_t alarm_count = { 0, 0, 0, 0 };

void decimalTime_to_charTime(Time_t * src, char * dst)
{
    dst[0] = INT_TO_CHAR(src->hour / 10);
    dst[1] = INT_TO_CHAR(src->hour % 10);
    dst[3] = INT_TO_CHAR(src->min / 10);
    dst[4] = INT_TO_CHAR(src->min % 10);
    dst[6] = INT_TO_CHAR(src->sec / 10);
    dst[7] = INT_TO_CHAR(src->sec % 10);
    dst[9] = INT_TO_CHAR(src->tsec);
}

void decimalDate_to_charDate(Date_t * src, char * dst)
{
    dst[0] = date_tbl[src->month - 1].month[0];
    dst[1] = date_tbl[src->month - 1].month[1];
    dst[2] = date_tbl[src->month - 1].month[2];
    dst[4] = INT_TO_CHAR(src->day / 10);
    dst[5] = INT_TO_CHAR(src->day % 10);

    int div10 = src->year / 10;
    int div100 = div10 / 10;
    int div1000 = div100 / 10;

    dst[10] = INT_TO_CHAR(src->year % 10);
    dst[9] = INT_TO_CHAR(div10 % 10);
    dst[8] = INT_TO_CHAR(div100 % 10);
    dst[7] = INT_TO_CHAR(div1000 % 10);
}

void send_result(char * src)
{
    update_cup(1, -atoi(cup.col));
    send_cup();
    send_str(src);
    update_cup(1, 0);
    send_cup();

}

/*************************************************************************
 struct String to store chars with length
 *************************************************************************/
typedef struct String String_t;
struct String
{
    char data[MAX_STR_SZ];
    int length;
    int cursor;
    int tmp_length;
};

void set_time(char * cmd_arg, Time_t * decimal_time)
{
    int h, m, s, t;
    char hour[3] = { cmd_arg[0], cmd_arg[1] };
    char min[3] = { cmd_arg[3], cmd_arg[4] };
    char sec[3] = { cmd_arg[6], cmd_arg[7] };
    char tsec[2] = { cmd_arg[9] };
    char punct[4] = { cmd_arg[2], cmd_arg[5], cmd_arg[8] };

    h = atoi(hour);
    m = atoi(min);
    s = atoi(sec);
    t = atoi(tsec);

    if (!IS_HOUR(h))
        send_result("INVALID HOUR");
    else if (!IS_MINUTE(m))
        send_result("INVALID MINUTE");
    else if (!IS_SEC(s))
        send_result("INVALID SECOND");
    else if (!IS_TSEC(t))
        send_result("INVALID TENTTH SECOND");
    else if (strcmp(punct, "::.") != 0)
        send_result("INVALID FORMAT");
    else
    {
        decimal_time->hour = h;
        decimal_time->min = m;
        decimal_time->sec = s;
        decimal_time->tsec = t;
        decimalTime_to_charTime(decimal_time, time);
        send_result(time);
    }
}

void time_func(char * cmd_arg)
{
    if (!cmd_arg)
    {
        decimalTime_to_charTime(&decimal_time, time);
        send_result(time);
    }
    else if (strlen(cmd_arg) == strlen(time))
    {
        set_time(cmd_arg, &decimal_time);
    }
    else
    {
        send_result("INVALID ARGS");
    }
}

void date_func(char * cmd_arg)
{
    if (!cmd_arg)
    {
        decimalDate_to_charDate(&decimal_date, date);
        send_result(date);
    }
    else if (strlen(cmd_arg) == strlen(date))
    {
        int y, m, d;
        char month[4] = { cmd_arg[0], cmd_arg[1], cmd_arg[2] };
        char day[3] = { cmd_arg[4], cmd_arg[5] };
        char year[5] = { cmd_arg[7], cmd_arg[8], cmd_arg[9], cmd_arg[10] };

        m = get_month(month);
        d = atoi(day);
        y = atoi(year);

        if (m == -1)
            send_result("INVALID MONTH");
        else if ((d < 1) || (d > date_tbl[m - 1].days))
            send_result("INVALID DAY");
        else if ((y < 1900) || (y > 9999))
            send_result("INVALID YEAR");
        else
        {
            decimal_date.year = y;
            decimal_date.month = m;
            decimal_date.day = d;
            decimalDate_to_charDate(&decimal_date, date);
            send_result(date);
        }
    }
    else
    {
        send_result("INVALID ARGS");
    }
}

void alarm_func(char * cmd_arg)
{
    if (!cmd_arg)
    {
        send_result("ALARM CLEAR");
        decimal_alarm.hour = 0;
        decimal_alarm.min = 0;
        decimal_alarm.sec = 0;
        decimal_alarm.tsec = 0;
    }
    else if (strlen(cmd_arg) == strlen(time))
    {
        set_time(cmd_arg, &decimal_alarm);
    }
    else
    {
        send_result("INVALID ARGS");
    }
}

void update_tick(Time_t * src, int tick, int * extra_day)
{
    int extra_sec, extra_min, extra_hour;

    extra_sec = (src->tsec + tick) / 10;
    extra_min = (src->sec + extra_sec) / 60;
    extra_hour = (src->min + extra_min) / 60;
    *extra_day = (src->hour + extra_hour) / 24;

    src->tsec = (src->tsec + tick) % 10;
    src->sec = (src->sec + extra_sec) % 60;
    src->min = (src->min + extra_min) % 60;
    src->hour = (src->hour + extra_hour) % 24;

}

void update_time(int tick, int * extra_day)
{
    update_tick(&decimal_time, tick, extra_day);

    if (decimal_alarm.hour || decimal_alarm.min || decimal_alarm.sec
            || decimal_alarm.tsec)
    {
        update_tick(&alarm_count, tick, extra_day);

        if ((decimal_alarm.hour == alarm_count.hour)
                && (decimal_alarm.min == alarm_count.min)
                && (decimal_alarm.sec == alarm_count.sec)
                && (decimal_alarm.tsec == alarm_count.tsec))
        {
            decimalTime_to_charTime(&decimal_alarm, time);
            send_result("ALARM EXPIRE");

            alarm_count.tsec = 0;
            alarm_count.sec = 0;
            alarm_count.min = 0;
            alarm_count.hour = 0;
            decimal_alarm.hour = 0;
            decimal_alarm.min = 0;
            decimal_alarm.sec = 0;
            decimal_alarm.tsec = 0;
        }
    }
}
void update_date(int extra_day)
{
    int extra_year, extra_mon;
    int d, m, y;

    d = decimal_date.day;
    m = decimal_date.month;
    y = decimal_date.year;

    if ((m == 2) && LEAP_YEAR(y))
    {
        extra_mon = (extra_day + d - 1) / (date_tbl[m - 1].days + 1);
        decimal_date.day = 1 + (d + extra_day - 1) % (date_tbl[m - 1].days + 1);
    }
    else
    {
        extra_mon = (extra_day + d - 1) / date_tbl[m - 1].days;
        decimal_date.day = 1 + (d + extra_day - 1) % date_tbl[m - 1].days;
    }

    extra_year = (m + extra_mon - 1) / 12;
    decimal_date.month = 1 + (m + extra_mon - 1) % 12;
    decimal_date.year = (y + extra_year) % 10000;
}

void reset_str(String_t * str)
{
    int i;

    for (i = 0; i < str->length; i++)
    {
        str->data[i] = '\0';
    }

    str->length = 0;
    str->cursor = 0;
    str->tmp_length = 0;
}

/*************************************************************************
 search the command table to find the input cmd,
 if not found, index = -1
 if found, call corresponding function
 *************************************************************************/
void get_cmd(String_t * str)
{
    char * cmd = strtok(str->data, " \t\n"); //get the first token of the input string
    int index;  //index in cmd table
    short length;   //length of cmd
    cmd_func func;  //function in cmd tbl

    get_cmdinfo(cmd, &index, &length, &func);   //get index, length, funciton

    if (index != -1)    //cmd found
    {
        char * arg = strtok(NULL, " "); //get the args of input string
        func(arg);  //call function (time, date, alarm)
    }
    else if (cmd)
    {
        send_result("COMMAND NOT FOUND");
    }
    else
    {
        update_cup(1, 0);
        send_cup();
    }
}

/*************************************************************************
 append a char to a string
 *************************************************************************/
void add_char(String_t * str, char data)
{

    if (str->tmp_length < MAX_STR_SZ)
    {
        if (str->tmp_length == str->length)
        {
            str->length += 1;
            str->cursor = (str->cursor + 1) % (MAX_COL + 1);
        }
        str->data[str->tmp_length] = data;
        str->tmp_length += 1;
    }
}

void remove_char(String_t * str)
{
    if (str->tmp_length > 0)
    {
        if (str->tmp_length == str->length)
        {
            str->length -= 1;
            str->cursor = (str->cursor - 1) % (MAX_COL + 1);
        }
        str->data[str->tmp_length - 1] = '\0';
        str->tmp_length -= 1;
    }
}

void vt100_sequence(String_t * str)
{
    char char1, char2;

    char1 = dequeue(UART_IN);
    char2 = dequeue(UART_IN);

    while (!char1)
    {
        char1 = dequeue(UART_IN);
    }
    while (!char2)
    {
        char2 = dequeue(UART_IN);
    }

    if (char1 == '[')
    {
        if ((char2 == 'D') && (str->tmp_length >= 1))   //left arrow
        {
            update_cup(0, -1);
            send_cup();
            str->tmp_length -= 1;
        }
        else if ((char2 == 'C') && (str->cursor > (atoi(cup.col) - 1))) //right arrow
        {
            update_cup(0, 1);
            send_cup();
            str->tmp_length += 1;
        }
    }
}

/*************************************************************************
 read a char UART0 input queue, if the char is not "Enter" key, write the
 char to the UART0 output queue. Else, call get_cmd to process the input
 string.
 *************************************************************************/
void UART0(void)
{
    char in_data = '\0'; //char from input queue
    static String_t str;    //string to be procebssed

    /* input queue not empty */

    if (!isEmpty(UART_IN))
    {
        in_data = dequeue(UART_IN);
        if (in_data == ESC) //escape sequence
        {
            vt100_sequence(&str);
        }
        else if (in_data == BACKSPACE)
        {
            remove_char(&str);
            send_char(in_data);
            update_cup(0, -1);

        }
        else if (in_data == ENTER)
        {
            get_cmd(&str);
            reset_str(&str);
        }
        else if (in_data == TAB)
        {
            send_char(in_data);
            add_char(&str, in_data);
            update_cup(0, 4);
        }
        else
        {
            send_char(in_data);
            add_char(&str, in_data);
            update_cup(0, 1);
        }
    }
}

void SYSTK(void)
{
    if (!isEmpty(SYSTK_Q))
    {
        int day = 0;

        dequeue(SYSTK_Q);
        update_time(TICK, &day);
        update_date(day);
    }
}

void main(void)
{
    UART0_Init();           // Initialize UART0
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts

    SysTickPeriod(MAX_WAIT);
    SysTickIntEnable();
    SysTickStart();

    InterruptMasterEnable();      // Enable Master (CPU) Interrupts

    /*update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
    update_cup(1,1);
    send_cup();
*/
    //send_char('A');
    /*send_str("hello");
    send_str("hello");
    send_str("hello");
    send_str("hello");
    send_str("hello");
    send_str("hello");*/

    while (1)
    {
        UART0();
        SYSTK();
    }
}

