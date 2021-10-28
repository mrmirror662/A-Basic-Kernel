#pragma once

#include <stdint.h>
#include <stdarg.h>
#define TEMP_ARRAY_SIZE 64
#define SLOW
enum COL : uint8_t
{
    BLACK = 0x0,
    BLUE = 0x1,
    GREEN = 0x2,
    CYAN = 0x3,
    RED = 0x4,
    MAGENTA = 0x5,
    BROWN = 0x6,
    LIGHT_GRAY = 0x7,
    DARK_GRAY = 0x8,
    LIGHT_BLUE = BLUE + 8,
    LIGHT_GREEN = GREEN + 8,
    LIGHT_CYAN = CYAN + 8,
    LIGHT_RED = RED + 8,
    LIGHT_MAGENTA = MAGENTA + 8,
    YELLOW = BROWN + 8,
    WHITE = LIGHT_GRAY + 8

};
class TextArea
{
public:
    void SetXY(int x, int y, uint16_t val);
    uint16_t GetXY(int x, int y);
    TextArea();
    int GetNumRows();
    int GetNumCols();

private:
    uint16_t *m_vidmem;
    uint32_t NumRows, NumCols;
};
class Console
{
public:
    void log(const char *);
    void logf(const char *...);
    void log(int);
    void log(char);
    void log(void *);
    void clear();
    void clearLine();
    void setCol(uint8_t, uint8_t);
    Console();
    friend void InitConsole();

private:
    TextArea area;
    uint8_t m_fcol;
    uint8_t m_bcol;

    static int cr, cc;
    static uint16_t chtoint(unsigned char ch, uint8_t fcol, uint8_t bcol);
    char *itos(int n);
    char *ptrtos(void *ptr);
    void scroll();
};
