//
//  Systick.c
//  queue
//
//  Created by John on 2019/9/22.
//  Copyright © 2019 John. All rights reserved.
//

#include "Systick.h"

// global variable to count number of interrupts on PORTF0 (falling edge)
volatile int st_overflow_count = 0;

void SysTickStart(void)
{
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
    // Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
    /*
     For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
     */
    ST_RELOAD_R = Period - 1; /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
    // Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
    // Clear the interrupt bit in STCTRL
    ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

void SysTickHandler(void)
{
    enqueue(SYSTK_Q, '\0');
}

