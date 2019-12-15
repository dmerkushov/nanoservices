/*
 * Copyright 2019 dmerkushov.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   testing.h
 * Author: dmerkushov
 *
 * Created on October 23, 2019, 4:33 PM
 */

#ifndef TESTING_H
#define TESTING_H

#include <iostream>

#define TEST_START startTest(__func__);
#define TEST_START_DESCRIBE(description) startTest(__func__, description);

/**
 * Starting call of a new test case
 * @param testName
 */
void startTest(const char *testName, const char *description = "");

/**
 * Indicate test passing
 */
void testPassed();

/**
 * Indicate test failure
 */
void testFailed();

/**
 * Publish the test results
 */
void testsResult();

#endif /* TESTING_H */

