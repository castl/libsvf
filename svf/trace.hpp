/*
 * LibSVF -- a library for computing Side-channel Vulnerability
 * Factor.
 *
 * Copyright (c) 2012, John Demme, Columbia University
 *                     jdd@cs.columbia.edu
 *                     
 *  Licensed under a BSD-3 license. See LICENSE for details
 */

#ifndef __SVF_TRACE_HPP__
#define __SVF_TRACE_HPP__

#include <vector>
#include <cassert>

namespace SVF {

template <typename Type>
class Trace {
    std::vector<Type> trace;

public:
    Trace() {
    }

    ~Trace() {
    }

    void push(Type point) {
        trace.push_back(point);
    }

    const Type& operator[](size_t i) {
        assert(i < trace.size());
        return trace[i];
    }

    size_t size() {
        return trace.size();
    }
}; // class Trace

}; // namespace SVF

#endif // __SVF_TRACE_HPP__
