//
//  Queue.c
//  queue
//
//  Created by John on 2019/9/20.
//  Copyright © 2019 John. All rights reserved.
//

#include "Queue.h"

static volatile Queue_t queue[QUEUE_NUM];


/*************************************************************************
 if queue is not full, write a char to the queue, inc the tail, inc the
 length.
 *************************************************************************/
void enqueue(int q_num, char data)
{
    int head = queue[q_num].head;
    head = (head + 1) & (QUEUE_SZ - 1);

    //UART0_IntDisable(UART_INT_TX);

    if (q_num == UART_OUT)
    {
        if ((head != queue[q_num].tail)) //queue not full
        {
            if (!data_xmitting(data)) //uart busy transimitting, queuing
            {
                queue[q_num].data[head] = data;
                queue[q_num].head = head;
            }
        }
    }
    else
    {
        if (head != queue[q_num].tail)  //uart busy transimitting, queuing
        {
            queue[q_num].data[head] = data;
            queue[q_num].head = head;
        }
    }

    //UART0_IntEnable(UART_INT_TX);

}

/*************************************************************************
 If queue is not empty, pop a char out of the queue; inc the head, dec the
 length; return the char. If empty, turn NULL.
 *************************************************************************/
char dequeue(int q_num)
{
    int tail = queue[q_num].tail;
    int data;

    //UART0_IntDisable(UART_INT_TX);

    if (queue[q_num].head == queue[q_num].tail)
    {
        //UART0_IntEnable(UART_INT_TX);
        return EMPTY;
    }
    else
    {
        queue[q_num].tail = (tail + 1) & (QUEUE_SZ - 1);
        tail = queue[q_num].tail;
        data = queue[q_num].data[tail];
        //UART0_IntEnable(UART_INT_TX);
        return data;
    }


}

bool isEmpty(int q_num)
{
    return queue[q_num].tail == queue[q_num].head;
}

bool isFull(int q_num)
{
    int head = queue[q_num].head;
    head = (head + 1) & (QUEUE_SZ - 1);

    return (head == queue[q_num].tail);
}

