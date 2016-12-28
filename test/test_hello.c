/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */

#include "unistd.h"
#include "string.h"

void reverse(char str[], int length);
char* itoa(int num, char* str, int base);

size_t str_len (const char *str)
{
    return (*str) ? str_len(++str) + 1 : 0;
}

int
main(int argc, const char* argv[])
{
    unsigned long stdout = 1;
    unsigned long ret = 0;

    //char * str = "Hello World\n";
    char * str2 = "Hello UCLA\n";
    char * str3 = "This is string 3!\n";
    char buf[20] = {0};
    unsigned long len = 0;
    
    for (int i = 0; i < argc; i++)
    {
        const char *str = argv[i];
        size_t len = str_len(str);
        write(stdout, str, len);
    }

    //__asm__("vmcall;\n");
    ret = write(stdout, str2, str_len(str2));
    itoa(ret, buf, 10);
    len = str_len(buf);
    buf[len] = '\n';
    ret = write(stdout, buf, len+1);
    ret = write(stdout, str3, str_len(str3));

    return ret;
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char tmp = str[end];
        str[end] = str[start]; 
        str[start] = tmp;
        start++;
        end--;
    }
}

char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 0;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}
