#include <iomanip>
#include <iostream>
#include <array>

#include "Date.h"

namespace minirisk {

struct DateInitializer : std::array<unsigned, Date::n_years>
{
    DateInitializer()
    {
        for (unsigned i = 0, s = 0, y = Date::first_year; i < size(); ++i, ++y) {
            (*this)[i] = s;
            s += 365 + (Date::is_leap_year(y) ? 1 : 0);
        }
    }
};

const std::array<unsigned, 12> Date::days_in_month = { {31,28,31,30,31,30,31,31,30,31,30,31} };
const std::array<unsigned, 12> Date::days_ytd{ {0,31,59,90,120,151,181,212,243,273,304,334} };
const std::array<unsigned, Date::n_years> Date::days_epoch(static_cast<const std::array<unsigned, Date::n_years>&>(DateInitializer()));

/* The function checks if a given year is a leap year.
    Leap year must be a multiple of 4, but it cannot be a multiple of 100 without also being a multiple of 400.
*/
bool Date::is_leap_year(unsigned year)
{
    return ((year % 4 != 0) ? false : (year % 100 != 0) ? true : (year % 400 != 0) ? false : true);
}

// The function pads a zero before the month or day if it has only one digit.
std::string Date::padding_dates(unsigned month_or_day)
{
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << month_or_day;
    return os.str();
}

// Get day, month and year as array from serial
std::array<unsigned, 3> Date::get_dmy(unsigned serial) const
{
    std::array<unsigned, 3> dmy;

    // calculate year
    unsigned y = static_cast<unsigned>(std::upper_bound(days_epoch.begin(), days_epoch.end(), serial) - days_epoch.begin() - 1);
    dmy[2] = first_year + y;

    serial -= days_epoch[y];
    bool leap = false;
    if (is_leap_year(dmy[2]) && serial > 58)
    {
        serial--;
        leap = true;
    }

    // calculate month
    unsigned m = static_cast<unsigned>(std::upper_bound(days_ytd.begin(), days_ytd.end(), serial) - days_ytd.begin() - 1);
    dmy[1] = m + 1;

    // calculate date
    if (m == 1 && leap)
    {
        dmy[0] serial - days_ytd[m] + 2;
    }
    else
    {
        dmy[0] = serial - days_ytd[m] + 1;
    }

    return dmy;
}

void Date::check_valid(unsigned serial)
{
    MYASSERT(serial >= 0 && serial <= 109572, "The serial must be a integer between 0 and 109572 (from 1-1-1900 to 31-12-2199), got " << serial);
}

void Date::check_valid(unsigned y, unsigned m, unsigned d)
{
    MYASSERT(y >= first_year, "The year must be no earlier than year " << first_year << ", got " << y);
    MYASSERT(y < last_year, "The year must be smaller than year " << last_year << ", got " << y);
    MYASSERT(m >= 1 && m <= 12, "The month must be a integer between 1 and 12, got " << m);
    unsigned dmax = days_in_month[m - 1] + ((m == 2 && is_leap_year(y)) ? 1 : 0);
    MYASSERT(d >= 1 && d <= dmax, "The day must be a integer between 1 and " << dmax << ", got " << d);
}

unsigned Date::day_of_year(unsigned y, unsigned m, unsigned d) const
{
    return days_ytd[m - 1] + ((m > 2 && is_leap_year(y)) ? 1 : 0) + (d - 1);
}


/*  The function calculates the distance between two Dates.
    d1 > d2 is allowed, which returns the negative of d2-d1.
*/
long operator-(const Date& d1, const Date& d2)
{
    unsigned s1 = d1.m_serial;
    unsigned s2 = d2.m_serial;
    return static_cast<long>(s1) - static_cast<long>(s2);
}

} // namespace minirisk

