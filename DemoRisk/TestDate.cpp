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

// Verify that the serial number generated for 2 contiguous dates are contiguous. For instance 31-Jan-2012 and
// 1-Feb-2012 are contiguous dates, hence the serial numbers they generate should only diﬀer by 1. Repeat for all
// pairs of contiguous dates in the valid range (1-Jan-1900, 31-Dec-2199).
void test3()
{
    int fail_count = 0;
    const std::array<unsigned, 12> days_in_month = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

    for (unsigned y = 1900; y < 2200; ++y)
    {
        for (unsigned m = 1; m <= 12; ++m)
        {
            unsigned dmax = days_in_month[m - 1] + ((m == 2 && Date::is_leap_year(y)) ? 1 : 0);
            for (unsigned d = 1; d < dmax; ++d) // loop stops at 30-Dec-2199 to compare it with 31-Dec-2199
            {
                Date date_current(y, m, d);

                // determine the next ymd
                unsigned y_1 = y;
                unsigned m_1 = m;
                unsigned d_1 = d + 1;

                if (d > dmax)
                {
                    d_1 = 1; // reset day to 1 if day > dmax

                    if (m == 12)
                    {
                        m_1 = 1;     // reset month to Jan
                        y_1 = y + 1; // increment to next year
                    }
                    else
                    {
                        m_1 = m + 1; // increment to the next month
                    }
                }
                Date date_next(y_1, m_1, d_1);

                if (date_next - date_current != 1)
                {
                    fail_count++;
                    std::cout << "The contiguous test for date " << d << "-" << m << "-" << y
                              << " and " << d_1 << "-" << m_1 << "-" << y_1 << " is failed. " << std::endl;
                }
            }
        }
    }

    if (fail_count == 0)
    {
        std::cout << "Test 3: SUCCESS" << std::endl;
    }
    else
    {
        throw std::runtime_error("Test 3 failed: Contiguous test failed.");
    }
}


int main()
{
    test1();
    test2();
    test3();
    return 0;
}
