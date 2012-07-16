#include <svf/svf.hpp>

#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <cmath>
#include <unordered_set>

using namespace std;

class SelectorTests: public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(SelectorTests);
    CPPUNIT_TEST(FullTriangleTest);
    CPPUNIT_TEST_SUITE_END();


protected:

    struct FakeTrace {
        size_t sz;
        FakeTrace(size_t sz) : sz(sz) { }

        size_t size() {
            return sz;
        }
    };

    void FullTriangleTest(void) {
        FakeTrace trace(500);
        SVF::FullTriangleSelector sel;
        sel.init(trace, trace);

        //printf("\n");
        for (size_t i=0; i<10; i++) {
            for (size_t j=0; j<i; j++) {
                boost::optional<SVF::Selector::Pair> p = sel.next();
                //printf("(%lu, %lu) (%lu, %lu)\n", p->first, p->second, i, j);
                CPPUNIT_ASSERT(p);
                CPPUNIT_ASSERT_EQUAL(i, p->first);
                CPPUNIT_ASSERT_EQUAL(j, p->second);
            }
        }
    }


};

CPPUNIT_TEST_SUITE_REGISTRATION(SelectorTests);
