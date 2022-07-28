#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../stringlist.h"
#include "../testutils.h"
#include "../../stringchunk.h"

int main(void)
{
    StringChunk c;

    char str[128];
    char Data[64];

    const char *si;
    char *di;

    int n;

    int Start = 0;

    srand(time(NULL));

    StringChunk_Init(&c, NULL);

    for( n = 70; n < sizeof(str); ++n )
    {
        StringChunk_Add(&c, RandomAlpha(str, sizeof(str)), RandomAlpha(Data, sizeof(Data)), sizeof(Data));
    }

    StringChunk_Add(&c, "a??.exe", "12345", 6);

    si = StringChunk_Enum_NoWildCard(&c, &Start, (void **)&di);
    while( si != NULL )
    {
        int h = StringChunk_Match(&c, si, NULL, (void **)&di);

        if( h )
        {
            printf("STRING : %s\nDATA: %s\n\n", si, di);
        } else {
            printf("STRING : %s\nNOT MATCHED\n\n", si);
        }

        si = StringChunk_Enum_NoWildCard(&c, &Start, (void **)&di);
    }

    int h = StringChunk_Match(&c, "asd1.exe", NULL, (void **)&di);
    if( h )
    {
        printf("STRING : %s\nDATA: %s\n\n", "asd1.exe", di);
    } else {
        printf("STRING : %s\nNOT MATCHED\n\n", "asd1.exe");
    }

    StringChunk_Free(&c, TRUE);

    return 0;
}
