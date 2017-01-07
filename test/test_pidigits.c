/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */

/*
 * Modified from:
 * https://cs.uwaterloo.ca/~alopez-o/math-faq/mathtext/node12.html
 * https://crypto.stanford.edu/pbc/notes/pi/code.html
 */

#include "stdlib.h"
#include "unistd.h"
#include "string.h"

int digits[1024];

int pidigits() {
    int r[2800 + 1];
    int i, k;
    int b, d;
    int c = 0;
    int m = 0;

    for (i = 0; i < 2800; i++) {
        r[i] = 2000;
    }

    for (k = 2800; k > 0; k -= 14) {
        d = 0;

        i = k;
        for (;;) {
            d += r[i] * 10000;
            b = 2 * i - 1;

            r[i] = d % b;
            d /= b;
            i--;
            if (i == 0) break;
            d *= i;
        }
        //printf("%.4d", c + d / 10000);
        digits[m++] = c + d / 10000;
        c = d % 10000;
    }

    return m;
}

int main(int argc, char* argv[])
{
    char buf[8] = {0};
    int n, m;
    int syscall = 1;

    if (argc > 1) n = atoi(argv[1]);
    else n = 100;

    if (argc > 2) syscall = 0;
    else syscall = 1;

    for (int i = 0; i < n; i++)
    {
        m = pidigits();
        for (int j = 0; j < m; j++)
        {
            itoa(digits[j], buf, 10);
            if (syscall) write(1, buf, strlen(buf));
        }
    }

    return 0;
}
