#include <iomanip>
#include <iostream>
#include <cstdlib>
#include "Date.h"
using namespace minirisk;

// Construction of an invalid date should generate an error: generate intentionally 1000 random invalid dates
// (e.g. 31-Apr-2010) and verify that the Date class constructor throws an error (use try...catch).
void test1()
{
    int num = 0;
    int total_tests = 1000;
    int catches = 0;

    srand(static_cast<long>(time(NULL)));

    while (num < total_tests)
    {
        num++;

        int y = rand() % 1000 + 1500;
        int m = rand() % 30 + 13; // invalid months from 13 onwards
        int d = rand() % 30 + 32; // invalid days from 32 onwards

        try
        {
            // Try Date construction, expecting failure
            Date(y, m, d);
        }
        catch (const std::invalid_argument &e)
        {
            // Count the exception catches
            catches++;
            continue;
        }
        catch (...)
        {
        }

        std::cout << "Failed to capture invalid date " << d << "-" << m << "-" << y << std::endl;
    }

    if (catches == total_tests)
    {
        std::cout << "Test 1: SUCCESS" << std::endl;
    }
    else
    {
        throw std::runtime_error("Test 1 failed: Not all invalid dates were caught.");
    }
}

int main()
{
    test1();
    test2();
    test3();
    return 0;
}
