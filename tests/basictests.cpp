#include <svf/svf.hpp>

#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>

#include <boost/math/special_functions/fpclassify.hpp>
#include <iostream>
#include <cmath>

#include <svf/timer.hpp>

#include <omp.h>

class ScalarTests: public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(ScalarTests);
    CPPUNIT_TEST(testExactInt);
    CPPUNIT_TEST(testExactIntMixed);
    CPPUNIT_TEST(testExactIntSampled);
    CPPUNIT_TEST(testExactDouble);
    CPPUNIT_TEST(testZeroCorrel);
    CPPUNIT_TEST(testRandomCorrel);
    CPPUNIT_TEST(testRandomCorrelBig);
    CPPUNIT_TEST(testPartialCorrel);
    //CPPUNIT_TEST(testRandomCorrelBigParallel);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct ScalarDist {
        template <typename T>
        double operator()(T a, T b) {
            assert(!isnan(a));
            assert(!isnan(b));
            return fabs(a - b);
        }
    };

    void testExactInt(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, i);
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void testExactIntMixed(void) {
        SVF::SVF<int, ScalarDist, double, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, ((double)i)/1000);
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void testExactIntSampled(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(i, i);
        }

        double svfVal = svf.computeSVF(SVF::RandomTraceProportional(25));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void testExactDouble(void) {
        SVF::SVF<double, ScalarDist, double, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            svf.pushTimestep(((double)i) / 10, ((double)i)/100);
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void testZeroCorrel(void) {
        SVF::SVF<double, ScalarDist, double, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            // Sine waves have zero linear correlation
            svf.pushTimestep(((double)i) / 10, sin((double)i));
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, svfVal, 0.005);
    }

    void testRandomCorrel(void) {
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        srand(100);
        for (size_t i=0; i<1000; i++) {
            // Sine waves have zero linear correlation
            svf.pushTimestep(rand(), rand());
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, svfVal, 0.05);
    }

   void testRandomCorrelBig(void) {
        // A bigger test for random correlation so we can lower the
        // delta
 
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        srand(100);
        for (size_t i=0; i<100000; i++) {
            // Sine waves have zero linear correlation
            svf.pushTimestep(rand(), rand());
        }

        double svfVal = svf.computeSVF(SVF::RandomTraceProportional(25));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, svfVal, 0.002);
    }

    void testPartialCorrel(void) {
        SVF::SVF<double, ScalarDist, double, ScalarDist> svf;

        for (size_t i=0; i<1000; i++) {
            // Sine waves have zero linear correlation, adding 'i'
            // makes it partially linearly correlated
            svf.pushTimestep(((double)i) / 10, i + i*sin((double)i));
        }

        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT(svfVal > 0.2);
        CPPUNIT_ASSERT(svfVal < 0.8);
    }

    void testRandomCorrelBigParallel(void) {
        // A bigger test for random correlation so we can lower the
        // delta
 
        SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

        srand(100);
        for (size_t i=0; i<100000; i++) {
            // Sine waves have zero linear correlation
            svf.pushTimestep(rand(), rand());
        }

        double lastSVF;
        for (size_t threads = 1; threads<16; threads++) {
            omp_set_num_threads(threads);
            printf("\n%lu threads: ", threads);
            fflush(stdout);
            double elapsed;
            double svfVal;
            {
                Timer t(elapsed);
                svfVal = svf.computeSVF(SVF::RandomTraceProportional(300));
            }
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, svfVal, 0.002);
            if (threads > 1) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(lastSVF, svfVal, 0.000001);
            }
            lastSVF = svfVal;

            printf("%lf seconds", elapsed);
        }
        printf("\n");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ScalarTests);
