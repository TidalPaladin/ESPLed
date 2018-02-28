#include "TestHelper.h"

unsigned long TestHelper::totalTestsRun = 0;
unsigned long TestHelper::totalTestsFailed = 0;
const char TestHelper::FAIL_TXT[] = "***** FAILED *****";
unsigned long TestHelper::testNumber = 0;


TestHelper::TestHelper(const char* method_name, const char* description)
:
METHOD_NAME(method_name),
DESCRIPTION(description)
{
	testNumber++;
	printHeader();
}



bool TestHelper::testResult(const char* expected, const char* actual) {
	if( !strcmp(expected, actual) ) {
		handlePassedTest();
		return true;
	}
	else {
		handleFailedTest();
		return false;
	}
}

bool TestHelper::testResult(std::string expected, std::string actual) {
	return testResult(expected.c_str(), actual.c_str());
}

bool TestHelper::printResult(bool expected, bool actual) {
	String expectedString = String(expected ? "true" : "false");
	String actualString = String(actual ? "true" : "false");

	testPrintF("Expected %s, Actual %s\n", expectedString.c_str(), actualString.c_str());
	return testResult(expected, actual);
}

bool TestHelper::printResult() {
	if( noneFailedThisTest() ) {
		testPrintF("%s - All tests passed!\n", METHOD_NAME);
		return true;
	}
	else {
		testPrintF("%s - %s - %i subtests\n", METHOD_NAME, FAIL_TXT, testsFailed);
		return false;
	}
}

bool TestHelper::end() {
	testPrintF("\n\nRan %i tests - ", TestHelper::totalTestsRun);

	if( allTestsPassed() ) { 
		testPrintF("All tests passed!\n");
	}
	else {
		testPrintF("%s %i tests\n", FAIL_TXT, totalTestsFailed);
	}
}












void TestHelper::printHeader() const {
	testPrintF("\n\nTest %i - %s - %s\n", testNumber, METHOD_NAME, DESCRIPTION);
}

void TestHelper::printFail() const {
	testPrintF("%s - %s - %s\n", FAIL_TXT, METHOD_NAME, DESCRIPTION);
}

void TestHelper::handleFailedTest() {
	testsFailed++;
	totalTestsFailed++;
	handlePassedTest();
}

void TestHelper::handlePassedTest() {
	totalTestsRun++;
	testsRun++;
}
