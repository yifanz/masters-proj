typedef unsigned long ssize_t;
typedef unsigned long size_t;

ssize_t write(int fd, const void *buf, size_t count);

void reverse(char str[], int length);
char* itoa(int num, char* str, int base);
size_t str_len (const char *str);

int
main(int argc, const char* argv[])
{
    unsigned long code = 0x2000004; // syscall "write"
    unsigned long stdout = 1;
    unsigned long ret = 0;

    char * str = "Hello World\n";
    char * str2 = "Hello UCLA\n";
    char str3[20];
    unsigned long len = 0;

    ret = write(stdout, str2, str_len(str2));
    itoa(ret, str3, 10);
    len = str_len(str3);
    str3[len] = '\n';
    //__asm__("vmcall;\n");
    ret = write(stdout, str3, len+1);

    /*
    __asm__(
            "movq %1, %%rax;\n"
            "movq %2, %%rdi;\n"
            "movq %3, %%rsi;\n"
            "movq %4, %%rdx;\n"
            "syscall;\n"
            "movq %%rax, %0;\n"
            : "=g"(ret)
            : "g"(code), "g"(stdout), "g"(str2), "g"(len2)
            : ); // assign from generic registers to syscall registers
    */

    return ret; // echo $?
}

ssize_t write(int fd, const void *buf, size_t count)
{
    long code = 0x2000004;
    unsigned long ret = 0;

    __asm__(/*
            "mov $0xcafe, %%rax;\n"
            "vmcall;\n"
            */
            "movq %1, %%rax;\n"
            "movq %2, %%rdi;\n"
            "movq %3, %%rsi;\n"
            "movq %4, %%rdx;\n"
            "syscall;\n"
            "movq %%rax, %0;\n"
            : "=g"(ret)
            : "g"(code), "g"(fd), "g"(buf), "g"(count)
            : );

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

size_t str_len (const char *str)
{
    return (*str) ? str_len(++str) + 1 : 0;
}

