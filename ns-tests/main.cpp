/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: dmerkushov
 *
 * Created on May 21, 2019, 6:20 PM
 */

#include "testing.h"
#include "testExceptionThrowThrough.h"
#include "testBase64.h"
#include "testBrokenWrite.h"

using namespace nanoservices::tests;

/*
 *
 */
int main(int argc, char **argv) {
	testBase64::testBase64();
	testExceptionThrowThrough::testExceptionThrowThrough();
	testBrokenWrite::testBrokenWrite();

	testsResult();

	return 0;
}

