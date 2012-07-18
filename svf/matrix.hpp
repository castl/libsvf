/*
 * LibSVF -- a library for computing Side-channel Vulnerability
 * Factor.
 *
 * Copyright (c) 2012, John Demme, Columbia University
 *                     jdd@cs.columbia.edu
 *                     
 *  Licensed under a BSD-3 license. See LICENSE for details
 */

#ifndef __SVF_MATRIX_HPP__
#define __SVF_MATRIX_HPP__

#include <svf/trace.hpp>

namespace SVF {

class Matrix {
public:
    Matrix() {
    }

    virtual ~Matrix() {
    }
    
    virtual double operator()(size_t i, size_t j) = 0; 
    double operator[](std::pair<size_t, size_t> p) {
        return (*this)(p.first, p.second);
    }
};

template <typename Type, typename Dist>
class LazyMatrix : public Matrix
{
    Trace<Type>& trace;
    Dist         dist;
public:
    LazyMatrix(Trace<Type>& trace): 
        trace(trace)
    { }

    double operator()(size_t i, size_t j) {
        return dist(trace[i], trace[j]);
    }
};


}; // namespace SVF
#endif // __SVF_MATRIX_HPP__
