#include <iostream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextOutputter.h>

#include <omp.h>

int runTests(size_t threads) {
    omp_set_num_threads(threads);

    printf("\n == Running all tests for %lu threads == \n\n", threads);

    //--- Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    //--- Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );        

    //--- Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener( &progress );      

    //--- Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( controller );

    CPPUNIT_NS::TextOutputter textOut(&result, std::cout);
    textOut.write();

    return result.wasSuccessful() ? 0 : 1;
}

int main( int ac, char **av )
{
    int rc = runTests(1);
    runTests(2);
    runTests(4);
    runTests(16);
    runTests(64);
    return rc;
}

