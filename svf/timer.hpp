/*
 * LibSVF -- a library for computing Side-channel Vulnerability
 * Factor.
 *
 * Copyright (c) 2012, John Demme, Columbia University
 *                     jdd@cs.columbia.edu
 *                     
 *  Licensed under a BSD-3 license. See LICENSE for details
 */

#ifndef __SVF_TIMER_HPP__
#define __SVF_TIMER_HPP__

#include <sys/time.h>

struct Timer {
    static double getTime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        double secs = tv.tv_usec;
        secs /= 1000000;
        secs += tv.tv_sec;
        return secs;
    }

    double startTime;
    double& timeResult;

    Timer(double& timeResult) :
        timeResult(timeResult) {
            startTime = getTime();
    }

    ~Timer() {
        double now = getTime();
        timeResult = now - startTime;
    }
};

#endif // __SVF_TIMER_HPP__
