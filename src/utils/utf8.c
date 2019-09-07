#include "utf8.h"

struct head {
    unsigned char bit_7 : 1;
    unsigned char bit_6 : 1;
    unsigned char bit_5 : 1;
    unsigned char bit_4 : 1;
    unsigned char bit_3 : 1;
    unsigned char bit_2 : 1;
    unsigned char bit_1 : 1;
    unsigned char bit_0 : 1;
} __attribute__((packed));

void utf8_width(const char *c, unsigned char *width)
{
    const struct head *head = (const struct head *)c;
    unsigned char w = 1;

    if(head->bit_0 == 0)
        w = 1;
    else {
        if(head->bit_1 == 1
            && head->bit_2 == 0) w = 2;
        else if(head->bit_1 == 1
            && head->bit_2 == 1
            && head->bit_3 == 0) w = 3;
        else if(head->bit_1 == 1
            && head->bit_2 == 1
            && head->bit_3 == 1
            && head->bit_4 == 0) w = 4;
    }

    *width = w;
}