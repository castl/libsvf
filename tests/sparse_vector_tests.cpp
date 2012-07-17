#include <svf/svf.hpp>
#include <svf/sparse_vector.hpp>

#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>

#include <svf/timer.hpp>

#include <iostream>
#include <cmath>

using namespace std;

class SparseVectorTests: public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(SparseVectorTests);
    CPPUNIT_TEST(EqualityTest);
    CPPUNIT_TEST(DistanceTest0);
    CPPUNIT_TEST(DistanceTest);
    CPPUNIT_TEST_SUITE_END();


protected:

    void EqualityTest(void) {
        SVF::SparseVector<double> a, b, c, d;

        for (size_t i=0; i<1000; i++) {
            a[i*2] = sin(i);
            b[i*2] = sin(i);
            c[i*2] = cos(i);
            d[i*3] = cos(i);
        }

        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(a == a);
        CPPUNIT_ASSERT(c == c);
        CPPUNIT_ASSERT(c != d);
        CPPUNIT_ASSERT(a != d);
        CPPUNIT_ASSERT(a != c);
        CPPUNIT_ASSERT(c != a);
        CPPUNIT_ASSERT(b != c);

        double tmp = b[0];
        b[0] = 100.0;
        CPPUNIT_ASSERT(a != b);
        b[0] = tmp;
        CPPUNIT_ASSERT(a == b);
        b[-1] = 0.0;
        CPPUNIT_ASSERT(a != b);
    }

    void DistanceTest0(void) {
        SVF::SparseVector<double> a, b;
        for (size_t i=0; i<1000; i++) {
            a[i*2] = sin(i);
            b[i*2] = sin(i);
        } 

        SVF::SparseVector<double>::EuclideanDistance<> dist;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dist(a, b), 0.00001);
    }

    void DistanceTest(void) {
        SVF::SparseVector<int64_t>::EuclideanDistance<> dist;
        SVF::SparseVector<int64_t> a, b;

        a[100] = 1;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, dist(a, b), 0.00001);

        b[100] = 1;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0, dist(a, b), 0.00001);

        b[50] = 1;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, dist(a, b), 0.00001);

        a[50] = 1;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0, dist(a, b), 0.00001);

        b[10] = 2;
        a[10] = 1;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1, dist(a, b), 0.00001);

        b[20] = 2;
        a[20] = 1;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(2), dist(a, b), 0.00001);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SparseVectorTests);

class SparseVectorSVFTests: public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(SparseVectorSVFTests);
    CPPUNIT_TEST(EqualityTest);
    CPPUNIT_TEST(EqualityTestMixed);
    CPPUNIT_TEST(EqualityTestDiffDims);
    CPPUNIT_TEST(EqualityTestDimReduction);
    CPPUNIT_TEST(EqualityTestMixedTrunc);
    //CPPUNIT_TEST(testRandomCorrelBigParallel);
    CPPUNIT_TEST_SUITE_END();


protected:

    SVF::SparseVector<double> randVec(size_t dims = 10) {
        SVF::SparseVector<double> a;
        for (size_t i=0; i<dims; i++) {
            a[i] = ((double)rand())/1000; 
        }
        return a;
    }

    void EqualityTest(void) {
        SVF::SVF<SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<>,
                 SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<> > svf;

        for (size_t i=0; i<100; i++) {
            auto a = randVec();
            svf.pushTimestep(a, a);
        }
        
        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void EqualityTestMixed(void) {
        SVF::SVF<SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<>,
                 SVF::SparseVector<int>, SVF::SparseVector<int>::EuclideanDistance<> > svf;

        for (size_t i=0; i<100; i++) {
            SVF::SparseVector<double> a;
            SVF::SparseVector<int> b;

            for (size_t d=0; d<10; d++) {
                int r = rand();
                a[d] = ((double)r) / 100;
                b[d] = r;
            }

            svf.pushTimestep(a, b);
        }
        
        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void EqualityTestDiffDims(void) {
        SVF::SVF<SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<>,
                 SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<> > svf;

        for (size_t i=0; i<100; i++) {
            SVF::SparseVector<double> a;
            SVF::SparseVector<double> b;

            for (size_t d=0; d<10; d++) {
                int r = rand();
                a[d] = ((double)r) / 100;
                b[d * 10 + 100] = ((double)r);
            }

            svf.pushTimestep(a, b);
        }
        
        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, svfVal, 0.0001);
    }

    void EqualityTestDimReduction(void) {
        SVF::SVF<SVF::SparseVector<int>, SVF::SparseVector<int>::EuclideanDistance<>,
                 SVF::SparseVector<int>, SVF::SparseVector<int>::EuclideanDistance<> > svf;

        for (size_t i=0; i<100; i++) {
            SVF::SparseVector<int> a;
            SVF::SparseVector<int> b;

            for (size_t d=0; d<20; d++) {
                int r = rand() % 1000;
                a[d] = r;
                b[d / 2] += r;
            }

            svf.pushTimestep(a, b);
        }
        
        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, svfVal, 0.3); // This delta is approx.
    }

    void EqualityTestMixedTrunc(void) {
        SVF::SVF<SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<>,
                 SVF::SparseVector<int>, SVF::SparseVector<int>::EuclideanDistance<> > svf;

        for (size_t i=0; i<100; i++) {
            SVF::SparseVector<double> a;
            SVF::SparseVector<int> b;

            for (size_t d=0; d<10; d++) {
                int r = rand() % 500;
                a[d] = ((double)r) / 100;
                b[d] = a[d];
            }

            svf.pushTimestep(a, b);
        }
        
        double svfVal = svf.computeSVF();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.95, svfVal, 0.05);
    }

    void testRandomCorrelBigParallel(void) {
        // A bigger test for random correlation so we can lower the
        // delta
        SVF::SVF<SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<>,
                 SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<> > svf;

        for (size_t i=0; i<5000; i++) {
            SVF::SparseVector<double> a = randVec(1000);
            SVF::SparseVector<double> b = randVec(1000);

            svf.pushTimestep(a, b);
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
                svfVal = svf.computeSVF(SVF::RandomTraceProportional(25));
            }
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, svfVal, 0.01);
            if (threads > 1) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(lastSVF, svfVal, 0.000001);
            }
            lastSVF = svfVal;

            printf("%lf seconds", elapsed);
        }
        printf("\n");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SparseVectorSVFTests);
