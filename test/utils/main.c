#include <string.h>
#include "../../utils.h"

int main(void)
{
    char a[2048] = "11223113445__566778811111111111990011223344551111122223333444455556666117777888899990000AAAASSSSDDDD!\0XZY";
    const char t[] = "11";

    const char T[] = "(abcdef)";

    printf("Length of a : %d\n", (int)strlen(a) + 1);
    printf("CountSubStr : %d\n", CountSubStr(a, t));

    printf("TotalSpaceNeeded : %d\n", (int)TOTAL_SPACE_NEEDED(a, strlen(t), strlen(T), CountSubStr(a, t)));
    ReplaceStr_WithLengthChecking(a, t, T, 167);
    printf("Length of a now : %d\n", (int)strlen(a) + 1);
    printf("\n%s\n", a);

    return 0;
}
