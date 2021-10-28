#include "Print.h"
int Console::cc = 0;
int Console::cr = 0;

Console::Console()
{
    m_fcol = COL::BLACK;
    m_bcol = COL::WHITE;
}

void Console::log(const char *msg)
{
    for (int i = 0; msg[i] != '\0'; i++)
    {
        log(msg[i]);
    }
#ifdef SLOW
    volatile int dummy = 0;
    for (uint64_t i = 0; i < 5000000; i++)
    {
        dummy++;
    }
#endif
}
void Console::clear()
{
    for (int i = 0; i < area.GetNumRows(); i++)
    {
        for (int j = 0; j < area.GetNumCols(); j++)
        {
            area.SetXY(j, i, chtoint(' ', m_fcol, m_bcol));
        }
    }
    cr = cc = 0;
}
void Console::log(char ch)
{

    switch (ch)
    {
    case '\n':
        cr++;
        cc = 0;
        return;
        break;
    case '\r':
        cc = 0;
        return;
        break;
    default:
        break;
    }

    if (cc > area.GetNumCols() && cr < area.GetNumRows())
    {
        cc = 0;
        cr++;
    }
    if (cr >= area.GetNumRows())
    {
        scroll();
        cr--;
        cc = 0;
    }
    area.SetXY(cc, cr, chtoint(ch, m_fcol, m_bcol));
    cc++;
}
uint16_t Console::chtoint(unsigned char ch, uint8_t fcol, uint8_t bcol)
{
    uint16_t attrib = (bcol << 4) | (fcol & 0x0F);
    return (uint16_t)(ch | (attrib << 8));
}
void Console::scroll()
{
    for (int i = 1; i < area.GetNumRows(); i++)
    {
        for (int j = 0; j < area.GetNumCols(); j++)
        {
            area.SetXY(j, i - 1, area.GetXY(j, i));
        }
    }
    for (int j = 0; j < area.GetNumCols(); j++)
    {
        area.SetXY(j, area.GetNumRows() - 1, chtoint(' ', m_fcol, m_bcol));
    }
}
void Console::logf(const char *msg...)
{
    va_list ptr;
    va_start(ptr, msg);

    for (int i = 0; msg[i] != '\0'; i++)
    {
        if (msg[i] == '%')
        {

            i++;
            if (msg[i] == 'x')
            {
                auto PTR = va_arg(ptr, void *);
                log(PTR);
            }
            if (msg[i] == 'i')
            {
                auto i = va_arg(ptr, int);
                log(i);
            }
            continue;
        }
        log(msg[i]);
    }
}
void Console::log(int num)
{
    log(itos(num));
}
void Console::log(void *ptr)
{
    log(ptrtos(ptr));
}
char *Console::itos(int n)
{
    static char num[TEMP_ARRAY_SIZE] = {'\0'};
    int numSize = 0;
    int n_ = n;
    if (n < 0)
    {
        n *= -1;
    }
    while (n != 0)
    {
        n /= 10;
        numSize++;
    }
    n = n_;
    if (n < 0)
    {
        n *= -1;
        numSize++;
        num[numSize + 1] = '\0';
        for (int i = numSize; i >= 1; i--)
        {
            auto dig = n % 10;
            n /= 10;
            num[i] = dig + 48;
        }
        num[0] = '-';
        return num;
    }
    num[numSize + 1] = '\0';
    for (int i = numSize; i >= 0; i--)
    {
        auto dig = n % 10;
        n /= 10;
        num[i] = dig + 48;
    }
    return num;
}
char *Console::ptrtos(void *ptr)
{
    static char hex[64];
    auto ptrVal = (uint64_t)ptr;
    uint32_t numSize = 0;
    auto _ptrVal = ptrVal;
    while (ptrVal != 0)
    {
        ptrVal /= 16;
        numSize++;
    }
    ptrVal = _ptrVal;
    hex[numSize + 3] = '\0';
    hex[0] = '0';
    hex[1] = 'x';
    for (int i = numSize + 2; i >= 2; i--)
    {
        auto dig = ptrVal % 16;
        ptrVal /= 16;
        if (dig > 10)
        {
            switch (dig)
            {
            case 10:
                hex[i] = 'a';
                break;
            case 11:
                hex[i] = 'b';
                break;
            case 12:
                hex[i] = 'c';
                break;
            case 13:
                hex[i] = 'd';
                break;
            case 14:
                hex[i] = 'e';
                break;
            case 15:
                hex[i] = 'f';
                break;
            default:
                break;
            }
            continue;
        }
        hex[i] = dig + 48;
    }
    return hex;
}

void Console::setCol(uint8_t fcol, uint8_t bcol = COL::WHITE)
{
    m_bcol = bcol;
    m_fcol = fcol;
}
void TextArea::SetXY(int x, int y, uint16_t val)
{
    auto index = x + y * NumCols;
    *(m_vidmem + index) = val;
}
uint16_t TextArea::GetXY(int x, int y)
{
    auto index = x + y * NumCols;
    return *(m_vidmem + index);
}
int TextArea::GetNumRows()
{
    return NumRows;
}
int TextArea::GetNumCols()
{
    return NumCols;
}

TextArea::TextArea()
{
    m_vidmem = (uint16_t *)0xb8000;
    NumCols = 80;
    NumRows = 25;
}