#include <svf/svf.hpp>

#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <cmath>
#include <unordered_set>

using namespace std;

class LFSRTests: public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(LFSRTests);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();


protected:

    void test(uint64_t mod) {
        unordered_set<uint64_t> returnedNums; 
        SVF::MaximalLFSRMod lfsr(mod);

        size_t iters = 100000;
        if (iters >= mod)
            iters = (mod -1);

        for (size_t i=0; i<iters; i++) {
            uint64_t n = lfsr.next();
            CPPUNIT_ASSERT(n < mod);
            //printf("\nn: %lu\n", n);
            CPPUNIT_ASSERT_EQUAL(0ul, returnedNums.count(n));
            returnedNums.insert(n);
        }
    }

    void test(void) {
        test(10);
        test(100);
        test(1234);
        test(12309);
        test(908712340);
        test(12349872349873498);
        test(23098479812);
    }


};

CPPUNIT_TEST_SUITE_REGISTRATION(LFSRTests);
