#ifndef __SVF_MATRIX_CORRELATE_HPP__
#define __SVF_MATRIX_CORRELATE_HPP__

#include <cassert>
#include <cmath>
#include <boost/optional.hpp>

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
    void init(Trace<OType>& oTrace, Trace<SCType>& scTrace) {
        traceSize = oTrace.size();
        reset();

        assert(traceSize == scTrace.size() && "Traces must be same size");
        assert(traceSize > 1 && "Traces must have at least two elements each");
    }

    virtual void reset() {
        x = 0;
        y = 0;
    }

    virtual boost::optional<Pair> next() {
        Pair ret(x++, y);
        if (x >= y) {
            if (y >= traceSize) {
                // We're at the end
                return boost::optional<Pair>();
            }

            // Go to next row
            y += 1;
            x = 0;
        }
        return ret;
    }
};

class RandomTraceProportional: public Selector {
    size_t traceSize;

public:
    RandomTraceProportional(double perTime) {
    }

    template<typename OType, typename SCType>
    void init(Trace<OType>& oTrace, Trace<SCType>& scTrace) {
        traceSize = oTrace.size();
        assert(traceSize == scTrace.size() && "Traces must be same size");

        assert(false && "Random matrix unimplemented");
    }

    virtual boost::optional<Pair> next() {
        assert(false);
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
            double yd = x[*p] - ybar;

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
