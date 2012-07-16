#include <svf/svf.hpp>

#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <cmath>

class ExactMatch : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(ExactMatch);
    CPPUNIT_TEST(testExactInt);
    CPPUNIT_TEST(testExactIntSampled);
    CPPUNIT_TEST_SUITE_END();

public:
    ExactMatch() {
    }

    ~ExactMatch() {
    }

    void setUp(void) {
    }

    void tearDown(void) {
    }

protected:

    struct ScalarDist {
        template <typename T>
        double operator()(T a, T b) {
            double ad = a;
            double ab = b;
            return sqrt(pow(ad, 2) + pow(ab, 2));
        }
    };

    void testExactInt(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, i);
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_EQUAL(svfVal, 1.0);
    }

    void testExactIntSampled(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, i);
        }

        //double svfVal = svf.computeSVF(SVF::RandomTraceProportional(25));
        //CPPUNIT_ASSERT_EQUAL(svfVal, 1.0);
        CPPUNIT_ASSERT(false);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ExactMatch);
