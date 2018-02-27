#ifndef __TEST_HELPER_H__
#define __TEST_HELPER_H__

#include <Arduino.h>
#include <string>
#include <cstdlib>
#include <ctime>

// Specify where to direct output to
#ifndef __TEST_HELPER_DEST__
#define __TEST_HELPER_DEST__    Serial
#endif

class TestHelper {

private:

    // The fail message
    static const char FAIL_TXT[];
    
    // Static variables to track testing for the entire program
    static unsigned long totalTestsRun;
    static unsigned long totalTestsFailed;
    static unsigned long testNumber;

    // Member variables to track testing for one method / TestHelper instance
    unsigned long testsRun = 0;
    unsigned long testsFailed = 0;
    
    // Member vars for designators
    const char* METHOD_NAME, *DESCRIPTION;

public:

    /**
     * @brief Constructor
     * 
     * @param method_name   A descriptive name of the method being tested
     * 
     * @param description   A description of the type of test being run
     * 
     * post: Header with method_name and description is printed, test counters are incremented
     */
    TestHelper(const char* method_name, const char* description);

    /**
     * @brief Gets the number of tests failed by this TestHelper instance
     * 
     * @return Number of times printResult() or testResult() returned false from this object
     */
    unsigned long getNumTestsFailed() const { return testsFailed; }

    /**
     * @brief Gets the number of tests run by this TestHelper instance
     * 
     * @return Number of times printResult() or testResult() was called by this object
     */
    unsigned long getNumTestsRun() const { return testsRun; }

    /**
     * @brief
     * 
     * @param
     * 
     */
    bool noneFailedThisTest() const { return getNumTestsFailed() == 0; }



    /**
     * @brief Gets the total number of tests failed over the couse of the program
     * 
     * 
     * @return Sum of tests failed by every TestHelper object
     */
    static unsigned long getTotalTestsFailed() { return totalTestsFailed; }


    /**
     * @brief Gets the total number of tests run over the couse of the program
     * 
     * 
     * @return Sum of tests run by every TestHelper object
     */
    static unsigned long getTotalTestsRun() { return totalTestsRun; }


    /**
     * @brief Gets whether every test has passed for this program run
     * 
     * @return true if numTestsFailed == 0 for every TestHelper created this program run
     * 
     */
    static bool allTestsPassed() { return getTotalTestsFailed() == 0;} 


    /**
     * @brief Compares expected and actual values using == for generic types
     * 
     * pre: expected and actual can be compared using ==
     * @param expected  The expected value
     * @param actual    The actual value
     * 
     * post:    getNumTestsRun()++, getTotalTestsRun()++,
     *          if(expected != actual) getNumTestsFailed()++, getTotalTestsFailed()++
     * 
     * @return expected == actual
     */
    template<typename T, typename U>
    bool testResult(T expected, U actual) {
        if(expected != actual) {
            handleFailedTest();
            return false;
        }
        else {
            handlePassedTest();
            return true;
        }
    }

    /**
     * @brief Compares expected and actual values within an allowable error
     * 
     * pre: expected and actual can be compared using ==
     * @param expected  The expected value
     * @param actual    The actual value
     * @param delta     The allowable difference betwee expected and actual
     * 
     * post:    getNumTestsRun()++, getTotalTestsRun()++,
     *          if(test failed) getNumTestsFailed()++, getTotalTestsFailed()++
     * 
     * @return abs(expected - actual) <= delta
     */
    template<typename T, typename U>
    bool testResultRange(T expected, U actual, double delta) {
        if( abs(expected - actual) > abs(delta) ) {
            handleFailedTest();
            return false;
        }
        else {
            handlePassedTest();
            return true;
        }
    }


    /**
     * @brief Compares expected and actual cstrings
     * 
     * pre: expected and actual can be compared using ==
     * @param expected  The expected value as a cstring
     * @param actual    The actual value as a cstring
     * 
     * post:    getNumTestsRun()++, getTotalTestsRun()++,
     *          if(expected != actual) getNumTestsFailed()++, getTotalTestsFailed()++
     * 
     * @return expected == actual
     */
    bool testResult(const char* expected, const char* actual);

    /**
     * @brief Compares expected and actual values using == for generic types
     * 
     * pre: expected and actual can be compared using ==
     * @param expected  The expected value
     * @param actual    The actual value
     * 
     * post:    getNumTestsRun()++, getTotalTestsRun()++,
     *          if(expected != actual) getNumTestsFailed()++, getTotalTestsFailed()++
     * 
     * @return expected == actual
     */
    bool testResult(std::string expected, std::string actual); 

    /**
     * @brief Calls testResult() and prints expected/actual values
     * 
     * @details     Values are printed by assigning them to an Arduino string. 
     *              ostringstream would be preferred
     * 
     * @param expected  The expeced value
     * @param actual    The actual value
     * 
     * post: expected and actual values printed
     * 
     * @return testResult(expected, actual);
     */
    template<typename T, typename U>
    bool printResult(T expected, U actual) {
        String expectedString = String(expected);
        String actualString = String(actual);

        testPrintF("Expected %s, Actual %s\n", expectedString.c_str(), actualString.c_str());
        return testResult(expected, actual);
    }
    bool printResult(bool expected, bool actual);

    /**
     * @brief Calls testResultRange() and prints expected/actual and delta values
     * 
     * @details     Values are printed by assigning them to an Arduino string. 
     *              ostringstream would be preferred
     * 
     * @param expected  The expeced value
     * @param actual    The actual value
     * @param delta     The allowed error of actual
     * 
     * post: print "(expected - delta) <= val <= (expected + delta)" and actual value
     * 
     * @return testResultRange(expected, actual, delta);
     */
    template<typename T, typename U>
    bool printResultRange(T expected, U actual, double delta) {
        String EXPECTED_STR_MIN = String(expected - delta);
        String EXPECTED_STR_MAX = String(expected + delta);
        String ACTUAL_STR = String(actual);

        testPrintF("Expected %s <= val <= %s, Actual %s\n",
            EXPECTED_STR_MIN.c_str(),
            EXPECTED_STR_MAX.c_str(),
            ACTUAL_STR.c_str()
        );
        return testResultRange(expected, actual, delta);
    }

    bool printResult();

    static bool end();

    // int randomBetween(int low, int high) {
    //     rand()
    // }




private:

    /**
     * @brief Prints the method name and test description
     * 
     */
    void printHeader() const;

    /**
     * @brief Prints the fail message
     * 
     */
    void printFail() const;
   
    /**
     * @brief Handles a failed test
     * 
     * post: totalTestsRun++, totalTestsFailed++, testsRun++ testsFailed++
     */
    void handleFailedTest();

    /**
     * @brief Handles a passed test
     * 
     * post: totalTestsRun++, testsRun++
     */
    void handlePassedTest();
 
    /**
     * @brief Sends printf output to the debug stream
     * 
     */
    template <typename... Args>
    static constexpr void testPrintF(const char* format, Args... args) {
        __TEST_HELPER_DEST__.printf(format, args...);
    }

    /**
     * @brief Sends println output to the debug stream
     * 
     */
    template <typename T>
    static constexpr void testPrintLine(T data) {
        __TEST_HELPER_DEST__.println(data);
    }


};

#endif