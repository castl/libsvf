#ifndef __SVF_MATRIX_CORRELATE_HPP__
#define __SVF_MATRIX_CORRELATE_HPP__

#include <cassert>
#include <cmath>
#include <stdint.h>
#include <boost/optional.hpp>

#include <cstdio>

namespace SVF {

class Selector {
public:
    typedef std::pair<size_t, size_t> Pair;
    virtual boost::optional<Pair> next() = 0;
    virtual void reset() = 0;
};

class FullTriangleSelector : public Selector {
    size_t traceSize;
    size_t x, y;

public:
    template<typename OType, typename SCType>
    void init(OType& oTrace, SCType& scTrace) {
        traceSize = oTrace.size();
        reset();

        assert(traceSize == scTrace.size() && "Traces must be same size");
        assert(traceSize > 1 && "Traces must have at least two elements each");
    }

    virtual void reset() {
        x = 0;
        y = 1;
    }

    virtual boost::optional<Pair> next() {
        if (y >= traceSize) {
            // We're at the end
            return boost::optional<Pair>();
        }

        Pair ret(y, x++);
        if (x >= y) {
            // Go to next row
            y += 1;
            x = 0;
        } 
        return ret;
    }
};

// Maximal LFSR tap bits, starting at 2^3.
// List generated with ../generateTaps.py
// Source:
//  http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
const uint64_t LFSR_TAPS[] = {
    0x0000000000000006ul,
    0x000000000000000cul,
    0x0000000000000014ul,
    0x0000000000000030ul,
    0x0000000000000060ul,
    0x00000000000000b8ul,
    0x0000000000000110ul,
    0x0000000000000240ul,
    0x0000000000000500ul,
    0x0000000000000829ul,
    0x000000000000100dul,
    0x0000000000002015ul,
    0x0000000000006000ul,
    0x000000000000d008ul,
    0x0000000000012000ul,
    0x0000000000020400ul,
    0x0000000000040023ul,
    0x0000000000090000ul,
    0x0000000000140000ul,
    0x0000000000300000ul,
    0x0000000000420000ul,
    0x0000000000e10000ul,
    0x0000000001200000ul,
    0x0000000002000023ul,
    0x0000000004000013ul,
    0x0000000009000000ul,
    0x0000000014000000ul,
    0x0000000020000029ul,
    0x0000000048000000ul,
    0x0000000080200003ul,
    0x0000000100080000ul,
    0x0000000204000003ul,
    0x0000000500000000ul,
    0x0000000801000000ul,
    0x000000100000001ful,
    0x0000002000000031ul,
    0x0000004400000000ul,
    0x000000a000140000ul,
    0x0000012000000000ul,
    0x00000300000c0000ul,
    0x0000063000000000ul,
    0x00000c0000030000ul,
    0x00001b0000000000ul,
    0x0000300003000000ul,
    0x0000420000000000ul,
    0x0000c00000180000ul,
    0x0001008000000000ul,
    0x0003000000c00000ul,
    0x0006000c00000000ul,
    0x0009000000000000ul,
    0x0018003000000000ul,
    0x0030000000030000ul,
    0x0040000040000000ul,
    0x00c0000600000000ul,
    0x0102000000000000ul,
    0x0200004000000000ul,
    0x0600003000000000ul,
    0x0c00000000000000ul,
    0x1800300000000000ul,
    0x3000000000000030ul,
    0x6000000000000000ul,
    0xd800000000000000ul
};

class MaximalLFSRMod {
    // A maximal LFSR with custom modulus

    uint64_t mod;
    uint64_t lfsr;
    uint64_t taps;

public:
    MaximalLFSRMod() {
        mod = 0;
    }

    MaximalLFSRMod(uint64_t mod) : mod(mod) {
        lfsr = 0xDEADBEEFDEADBEEF % mod;
        size_t bits = 3;
        while ((1ul << bits) < mod)
            bits++;
        this->taps = LFSR_TAPS[bits - 3];
        //printf("MaxLFSR: %lu, %lu, %lu, 0x%lx\n", bits, mod, lfsr, taps);
    }

    uint64_t next() {
        do {
            // Generate next
            lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & taps);

            // Keep generating while above our mod
        } while (lfsr >= mod); 
        return lfsr;
    }
};


class RandomTraceProportional: public Selector {
    MaximalLFSRMod lfsr;
    double perTime;
    size_t traceSize;
    size_t numSamples;
    size_t num;
    size_t full;

public:
    RandomTraceProportional(double perTime) : perTime(perTime) {
    }

    template<typename OType, typename SCType>
    void init(Trace<OType>& oTrace, Trace<SCType>& scTrace) {
        traceSize = oTrace.size();
        numSamples = perTime * traceSize;
        full = (traceSize * (traceSize - 1)) / 2;
        if (numSamples > full)
            numSamples = full;
        reset();

        assert(traceSize == scTrace.size() && "Traces must be same size");
        assert(traceSize > 1 && "Traces must have at least two elements each");
    }

    void reset() {
        num = 0;
        lfsr = MaximalLFSRMod(full);
    }

    virtual boost::optional<Pair> next() {
        if (num >= numSamples) {
            return boost::optional<Pair>();
        }
        num += 1;
        uint64_t r = lfsr.next();
        double rD = r;
        uint64_t j = floor(sqrt(0.25 + 2*rD) - 0.5);
        uint64_t i = r - j*(1+j)/2;
        j += 1;
        assert(j < traceSize);
        assert(i < traceSize);
        return Pair(i, j);
    }
};


class Pearson {
public:
    double operator()(Matrix& x, Matrix& y, Selector& sel) {
        boost::optional<Selector::Pair> p;

        size_t size = 0;
        double xbar = 0.0, ybar = 0.0;

        sel.reset();
        while ( (p = sel.next()) ) {
            size += 1;
            xbar += x[*p];
            ybar += y[*p];
        }

        // Compute averages
        xbar /= size;
        ybar /= size;

        sel.reset();
        size_t secondCounter = 0;
        double sum = 0.0, sx = 0.0, sy = 0.0;
        while (p = sel.next()) {
            secondCounter += 1;

            double xd = x[*p] - xbar;
            double yd = y[*p] - ybar;

            sum += xd * yd;
            sx += xd * xd;
            sy += yd * yd;
        }
        assert(secondCounter == size);

        return sum / (sqrt(sx) * sqrt(sy));
    }
};

}; // namespace SVF
#endif // __SVF_MATRIX_CORRELATE_HPP__
