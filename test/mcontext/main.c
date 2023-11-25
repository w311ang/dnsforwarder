#include "../../common.h"
#include "../../mcontext.h"
#include <stdlib.h>
#include <time.h>

typedef struct {
    IHeader h;
    uint16_t    i;
} HH;

MsgContext *MakeOne(void)
{
    static int s = 0;

    static HH h;

    h.h.HashValue = s++;

    h.i = s++;

    return (MsgContext *)&h;
}

int main(void)
{
    HH a, b;

    ModuleContext   c;

    ModuleContext_Init(&c, sizeof(HH));

    a = *(HH *)MakeOne();

    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, (MsgContext *)&a);
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());
    c.Add(&c, MakeOne());

    c.GenAnswerHeaderAndRemove(&c, (MsgContext *)&a, (MsgContext *)&b);

    return 0;
}
