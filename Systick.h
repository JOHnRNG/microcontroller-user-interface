//
//  Systick.h
//  queue
//
//  Created by John on 2019/9/22.
//  Copyright © 2019 John. All rights reserved.
//

#ifndef Systick_h
#define Systick_h

#include <stdio.h>
#include "Queue.h"
/*
 - SysTick sample code
 - Originally written for the Stellaris (2013)
 - Will need to use debugger to "see" interrupts
 - Code uses bit-operations to access SysTick bits
 */
// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

// Maximum period
#define MAX_WAIT           0x186A00   /* 1600000 */

#define TRUE    1
#define FALSE   0

#define TICK 1  //0.1 second


/* Global to signal SysTick interrupt */
extern volatile int st_overflow_count;

void SysTickStart(void);
void SysTickStop(void);
void SysTickPeriod(unsigned long Period);
void SysTickIntEnable(void);
void SysTickIntDisable(void);
void SysTickHandler(void);

#endif /* Systick_h */
