//
//  CmdTbl.c
//  queue
//
//  Created by John on 2019/9/22.
//  Copyright  2019 John. All rights reserved.
//

#include "CmdTbl.h"

Cmd_t cmd_tbl[CMDTBL_SIZE] =
{
    {"TIME", 4, time_func},
    {"DATE", 4, date_func},
    {"ALARM", 5, alarm_func},
};

void get_cmdinfo(char * cmd, int * index, short * length, cmd_func * func)
{
    *index = -1;
    int i = 0;
    
    for (i = 0; i < CMDTBL_SIZE; i ++)
    {
        if (strcasecmp(cmd, cmd_tbl[i].mnemonic) == 0)
        {
            *index = i;
            *length = cmd_tbl[i].length;
            *func = cmd_tbl[i].func;
        }
    }
}
