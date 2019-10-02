//
//  CmdTbl.h
//  queue
//
//  Created by John on 2019/9/22.
//  Copyright © 2019 John. All rights reserved.
//

#ifndef CmdTbl_h
#define CmdTbl_h

#include <stdio.h>
#include <strings.h>

#define CMD_SIZE 10
#define CMDTBL_SIZE 3


typedef struct Cmd Cmd_t;
typedef void (*cmd_func) (char * cmd);

struct Cmd
{
    char mnemonic [CMD_SIZE];
    short length;
    cmd_func func;
};

extern Cmd_t cmd_tbl[CMDTBL_SIZE];

extern void time_func(char * cmd_arg);
extern void date_func(char * cmd_arg);
extern void alarm_func(char * cmd_arg);
void get_cmdinfo(char * cmd, int * index, short * length, cmd_func * func);

#endif /* CmdTbl_h */
