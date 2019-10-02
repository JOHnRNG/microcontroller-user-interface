//
//  Queue.h
//  queue
//
//  Created by John on 2019/9/20.
//  Copyright © 2019 John. All rights reserved.
//

#ifndef Queue_h
#define Queue_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define QUEUE_SZ 32
#define QUEUE_NUM 3
#define UART_IN 0
#define UART_OUT 1
#define SYSTK_Q 2

#define FULL 0
#define EMPTY 0

/*************************************************************************
 struct of a circular queue, char * data is the array.
 *************************************************************************/
typedef struct Queue Queue_t;
struct Queue
{
    char data[QUEUE_SZ];
    int head;
    int tail;
};


/* write to the queue */
void enqueue(int q_num, char data);

/* read from the queue */
char dequeue(int q_num);

bool isEmpty(int q_num);

bool isFull(int q_num);

extern int data_xmitting(char data);

#endif /* Queue_h */
