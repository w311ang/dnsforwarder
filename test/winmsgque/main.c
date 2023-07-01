#define _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../winmsgque.h"

void t(void *a)
{
    WinMsgQue *q = (WinMsgQue *)a;
    srand(time(NULL));
    while( TRUE )
    {
        int i = rand();
        q->Post(q, &i);
        printf("***Posted number %d.\n", i);
        SLEEP(1000);
    }
}

void p(WinMsgQue *q)
{
    int n = 10;
    while( n-- != 0 )
    {
        int i = rand();
        q->Post(q, &i);
    }
}

int main(void)
{
    WinMsgQue q;
    ThreadHandle th;

    WinMsgQue_Init(&q, sizeof(int));

    CREATE_THREAD(t, &q, th);
    DETACH_THREAD(th);
    CREATE_THREAD(t, &q, th);
    DETACH_THREAD(th);
    CREATE_THREAD(t, &q, th);
    DETACH_THREAD(th);
    CREATE_THREAD(t, &q, th);
    DETACH_THREAD(th);
    CREATE_THREAD(t, &q, th);
    DETACH_THREAD(th);
    CREATE_THREAD(t, &q, th);
    DETACH_THREAD(th);

    while( TRUE )
    {
        int *i;
        DWORD tv = 1000;
        i = q.Wait(&q, &tv);
        if( i == NULL )
        {
            printf("-->Didn't get anything.\n");
        } else {
            printf("-->Get number %d.\n", *i);
        }
    }

    return 0;
}
