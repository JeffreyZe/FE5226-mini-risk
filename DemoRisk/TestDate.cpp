#include <iomanip>
#include <iostream>
#include <cstdlib>
#include "Date.h"
using namespace minirisk;

// Construction of an invalid date should generate an error: generate intentionally 1000 random invalid dates
// (e.g. 31-Apr-2010) and verify that the Date class constructor throws an error (use try...catch).
void test1()
{
    int count = 0;
    int total_tests = 1000;
    int exception_count = 0;

    srand(static_cast<long>(time(NULL)));

    while (count < total_tests)
    {
        count++;

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
            // Count the exception exception_count
            exception_count++;
            continue;
        }
        catch (...)
        {
        }

        std::cout << "Failed to capture invalid date " << d << "-" << m << "-" << y << std::endl;
    }

    if (exception_count == total_tests)
    {
        std::cout << "Test 1: SUCCESS" << std::endl;
    }
    else
    {
        throw std::runtime_error("Test 1 failed: Not all invalid dates were caught.");
    }
}

// Verify that converting a date in calendar format (day, month, year) to serial format and then converting back to
// calendar format yields the original date. Repeat for all dates in the valid range (1-Jan-1900, 31-Dec-2199).
void test2()
{
    int fail_count = 0;
    const std::array<unsigned, 12> days_in_month = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

    // loop through all days from 1-Jan-1900 to 31-Dec-2199
    for (unsigned y = 1900; y < 2200; ++y)
    {
        for (unsigned m = 1; m <= 12; ++m)
        {
            unsigned dmax = days_in_month[m - 1] + ((m == 2 && Date::is_leap_year(y)) ? 1 : 0);
            for (unsigned d = 1; d <= dmax; ++d)
            {

                // to serial format
                Date date_obj(y, m, d);
                // to calendar format
                std::string date_string = date_obj.to_string();

                // check whether still the same
                std::string calendar_format = std::to_string(d) + '-' + std::to_string(m) + '-' + std::to_string(y);
                if (calendar_format != date_string)
                {
                    fail_count++;
                    std::cout << "The conversion for Date " << d << "-" << m << "-" << y << " is failed." << std::endl;
                }
            }
        }
    }

    if (fail_count == 0)
    {
        std::cout << "Test 2: SUCCESS" << std::endl;
    }
    else
    {
        throw std::runtime_error("Test 2 failed: Calendar and Serial format conversion failed.");
    }
}

int main()
{
    test1();
    test2();
    test3();
    return 0;
}
