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
    CPPUNIT_TEST(testExactDouble);
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
            return sqrt(pow((double)a, 2) + pow((double)b, 2));
        }
    };

    void testExactInt(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, i);
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(svfVal, 1.0, 0.001);
    }

    void testExactIntSampled(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, i);
        }

        double svfVal = svf.computeSVF(SVF::RandomTraceProportional(25));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(svfVal, 1.0, 0.001);
    }

    void testExactDouble(void) {
        SVF::SVF<double, ScalarDist, double, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(((double)i) / 10, ((double)i)/100);
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(svfVal, 1.0, 0.001);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ExactMatch);
