/*
 * LibSVF -- a library for computing Side-channel Vulnerability
 * Factor.
 *
 * Copyright (c) 2012, John Demme, Columbia University
 *                     jdd@cs.columbia.edu
 *                     
 *  Licensed under a BSD-3 license. See LICENSE for details
 */

#ifndef __SVF_MATRIX_CORRELATE_HPP__
#define __SVF_MATRIX_CORRELATE_HPP__

#include <cassert>
#include <cmath>
#include <stdint.h>
#include <boost/optional.hpp>
#include <omp.h>

#include <cstdio>

namespace SVF {

class Selector {
public:
    virtual ~Selector() {}

    typedef std::pair<size_t, size_t> Pair;
    virtual boost::optional<Pair> next() = 0;
    virtual boost::optional<Pair> next(int num) {
        assert(num > 0);
        auto r = next();
        advance(num - 1);
        return r;
    }
    virtual void advance(size_t num) {
        for (size_t i=0; i<num; i++)
            next();
    }
    virtual void reset() = 0;
    virtual Selector* clone() = 0;
};

class FullTriangleSelector : public Selector {
    size_t traceSize;
    size_t x, y;

public:
    FullTriangleSelector() {
    }

    FullTriangleSelector(const FullTriangleSelector& c) {
        this->traceSize = c.traceSize;
        this->x = c.x;
        this->y = c.y;
    }

    template<typename OType, typename SCType>
    void init(OType& oTrace, SCType& scTrace) {
        traceSize = oTrace.size();
        reset();

        assert(traceSize == scTrace.size() && "Traces must be same size");
        assert(traceSize > 1 && "Traces must have at least two elements each");
    }

    void reset() {
        x = 0;
        y = 1;
    }

    boost::optional<Pair> next() {
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

    void advance(size_t num) {
        for (size_t i=0; i<num; i++)
            next();
    }

    boost::optional<Pair> next(int num) {
        assert(num > 0);
        auto r = next();
        advance(num - 1);
        return r;
    }

    FullTriangleSelector* clone() {
        return new FullTriangleSelector(*this);
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

    MaximalLFSRMod(const MaximalLFSRMod& c) {
        mod = c.mod;
        lfsr = c.lfsr;
        taps = c.taps;
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

    RandomTraceProportional(const RandomTraceProportional& c) {
        lfsr = c.lfsr;
        perTime = c.perTime;
        traceSize = c.traceSize;
        numSamples = c.numSamples;
        num = c.num;
        full = c.full;
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

    boost::optional<Pair> next() {
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

    void advance(size_t num) {
        for (size_t i=0; i<num; i++)
            next();
    }

    boost::optional<Pair> next(int num) {
        assert(num > 0);
        auto r = next();
        advance(num - 1);
        return r;
    }

    virtual RandomTraceProportional* clone() {
        return new RandomTraceProportional(*this);
    }
};

#define TBUFFER 500 

class Pearson {
public:
    double operator()(Matrix& x, Matrix& y, Selector& sel) {

        size_t size = 0;
        double xbar = 0.0, ybar = 0.0;

        sel.reset();
#pragma omp parallel default(none) shared(x, y, sel) reduction(+:size) reduction(+:xbar) reduction(+:ybar)
        {
            // Get a selector clone, advance by my thread number
            size_t adv = omp_get_num_threads();
            size_t thread = omp_get_thread_num();
            //printf("Thread: %lu, adv:%lu\n", thread, adv);
            Selector* mySel = sel.clone();
            mySel->advance(thread);

            boost::optional<Selector::Pair> p;
            while ( (p = mySel->next(adv)) ) {
                size += 1;
                auto pl = *p;
                xbar += x[pl];
                ybar += y[pl];
            }

            delete mySel;
        }


        // Compute averages
        xbar /= size;
        ybar /= size;

        size_t secondCounter = 0;
        double sum = 0.0, sx = 0.0, sy = 0.0;

#pragma omp parallel default(none) shared(x, y, sel, xbar, ybar) \
        reduction(+:secondCounter) reduction(+:sum) reduction(+:sx) reduction(+:sy)
        {
            // Get a selector clone, advance by my thread number
            size_t adv = omp_get_num_threads();
            size_t thread = omp_get_thread_num();
            //printf("Thread: %lu, adv:%lu\n", thread, adv);
            Selector* mySel = sel.clone();
            mySel->advance(thread);

            boost::optional<Selector::Pair> p;
            while ( (p = mySel->next(adv)) ) {
                secondCounter += 1;

                double xd = x[*p] - xbar;
                double yd = y[*p] - ybar;

                sum += xd * yd;
                sx += xd * xd;
                sy += yd * yd;
            }

            delete mySel;
        }

        //printf("SecondCounter: %lu, Size: %lu, xbar: %lf, ybar: %lf\n", 
        //secondCounter, size, xbar, ybar);
        assert(secondCounter == size);

        return sum / (sqrt(sx) * sqrt(sy));
    }
};

}; // namespace SVF
#endif // __SVF_MATRIX_CORRELATE_HPP__
