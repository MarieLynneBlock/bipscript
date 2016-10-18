#ifndef TIMESIGNATURE_H
#define TIMESIGNATURE_H

class TimeSignature
{
    bool valid;
    uint32_t numerator;
    uint32_t denominator;
public:
    TimeSignature() :
        valid(false), numerator(0), denominator(0) {}
    TimeSignature(bool v, uint32_t n, uint32_t d) :
        valid(v), numerator(n), denominator(d) {}
    TimeSignature(uint32_t n, uint32_t d) :
        valid(true), numerator(n), denominator(d) {}
    bool isValid() {
        return valid;
    }
    uint32_t getNumerator() {
        return numerator;
    }
    uint32_t getDenominator() {
        return denominator;
    }
};

#endif // TIMESIGNATURE_H
