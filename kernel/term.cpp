#include "term.h"

static volatile limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0};

limine_terminal *g_terminal = nullptr;

void init_term()
{
    // Ensure we got a terminal
    if (terminal_request.response == NULL || terminal_request.response->terminal_count < 1)
    {
        hlt();
    }

    g_terminal = terminal_request.response->terminals[0];
}
void print(const char *s)
{
    terminal_request.response->write(g_terminal, s, strlen(s));
}
void printc(char c)
{
    char s[2];
    s[0] = c;
    s[1] = '\0';
    terminal_request.response->write(g_terminal, s, strlen(s));
}
void print(uint64_t i)
{
    char num[18];
    uint16_t size = 0;
    uint16_t counter = 0;
    if (i == 0)
    {
        print("0");
        return;
    }
    while (i > 0)
    {
        uint8_t dig = i % 10;
        i = i / 10;
        num[counter] = '0' + dig;
        counter++;
    }
    for (uint16_t i = 0; i < counter / 2; i++)
    {
        auto tmp = num[i];
        num[i] = num[counter - 1 - i];
        num[counter - 1 - i] = tmp;
    }
    num[counter] = '\0';
    print((const char *)num);
}
void print(void *ptr)
{
    uint64_t i = (uint64_t)ptr;
    if (i == 0)
    {
        print("0");
        return;
    }
    char buff[18];
    uint16_t size = 0;
    uint16_t counter = 0;
    buff[0] = '0';
    buff[1] = 'x';
    char *num = buff + 2;
    while (i > 0)
    {
        uint8_t dig = i % 16;
        i = i / 16;
        if (dig < 10)
            num[counter] = '0' + dig;
        else
            num[counter] = 'a' - 10 + dig;
        counter++;
    }
    for (uint16_t i = 0; i < counter / 2; i++)
    {
        auto tmp = num[i];
        num[i] = num[counter - 1 - i];
        num[counter - 1 - i] = tmp;
    }
    num[counter] = '\0';
    print((const char *)buff);
}
void printf(const char *msg, ...)
{
    va_list ptr;
    va_start(ptr, msg);

    auto N = strlen(msg);
    for (size_t i = 0; i < N; i++)
    {
        if (msg[i] == '%')
        {
            i++;
            if (msg[i] == 'x')
            {
                auto PTR = va_arg(ptr, void *);
                print(PTR);
            }
            if (msg[i] == 'i')
            {
                auto i = va_arg(ptr, uint64_t);
                print(i);
            }
        }
        else
            printc(msg[i]);
    }
}