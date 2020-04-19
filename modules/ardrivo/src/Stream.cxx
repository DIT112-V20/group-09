#include <cctype>
#include "Stream.h"

void Stream::setTimeout(long timeout) { _timeout = timeout; }
bool Stream::findUntil(char* target, int length, char terminal) noexcept {
    int count = -1;
    while (true) {
        int c = read();
        if (c < 0 || c == terminal)
            break;
        if (c == *target)
            ++count;
        else
            count = -1;
        if (count == length)
            return true;
    }
    return false;
}

int Stream::peekNextDigit(LookaheadMode lookahead, bool detectDecimal) {
    int c;
    while (true) {
        c = peek();

        if (c < 0 || c == '-' || std::isdigit(c) || (detectDecimal && c == '.'))
            return c;

        switch (lookahead) {
        case SKIP_NONE:
            return -1;
        case SKIP_WHITESPACE:
            switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            default:
                return -1;
            }
        case SKIP_ALL:
            break;
        }
        read();
    }
}
float Stream::parseFloat(LookaheadMode lookahead, char ignore) {
    bool isNegative = false;
    bool isFraction = false;
    long value = 0;
    float fraction = 1.0;
    int c = peekNextDigit(lookahead, true);

    if (c < 0)
        return 0;

    do {
        if (c == ignore)
            ;
        else if (c == '-')
            isNegative = true;
        else if (c == '.')
            isFraction = true;
        else if (std::isdigit(c)) {
            value = value * 10 + c - '0';
            if (isFraction)
                fraction *= 0.1;
        }
        read();
        c = peek();
    } while (std::isdigit(c) || (c == '.' && !isFraction) || c == ignore);

    if (isNegative)
        value = -value;

    return isFraction ? value * fraction : value;
}

long Stream::parseInt(LookaheadMode lookahead, char ignore) {
    bool isNegative = false;
    long value = 0;

    int c = peekNextDigit(lookahead, false);
    if (c < 0)
        return 0;

    do {
        if (c == ignore)
            ;
        else if (c == '-')
            isNegative = true;
        else if (std::isdigit(c))
            value = value * 10 + c - '0';
        read();
        c = peek();
    } while (std::isdigit(c) || c == ignore);

    if (isNegative)
        value = -value;
    return value;
}

size_t Stream::readBytesUntil(char terminator, char* buffer, int length) {
    size_t index = 0;
    while (index < length) {
        int c = read();
        if (c < 0 || c == terminator)
            break;
        *buffer++ = static_cast<char>(c);
        ++index;
    }
    return index;
}

String Stream::readStringUntil(char terminator) {
    String ret;
    while (true) {
        int c = read();
        if (c < 0 || c == terminator)
            break;
        ret.concat(static_cast<char>(c));
    }
    return ret;
}
