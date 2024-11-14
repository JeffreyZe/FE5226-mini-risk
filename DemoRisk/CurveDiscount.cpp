#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

CurveDiscount::CurveDiscount(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today)
    , m_name(curve_name)
    , m_rate(mkt->get_yield(curve_name.substr(ir_curve_discount_prefix.length(),3)))
{
}

double CurveDiscount::df(const Date& t) const
{
    MYASSERT((!(t < m_today)), "cannot get discount factor for date in the past: " << t);
    auto m_rate_last = --m_rate.end();
    long max_last_day = static_cast<long>(m_rate_last->first);
    long day_diff = t - m_today;
    MYASSERT((!(day_diff > max_last_day)), "cannot get discount factor for date after last tensor date : " << t);
    auto df = m_rate.lower_bound(day_diff); // pointing to Ti+1
    unsigned t2 = df->first;
    double rt2 = df->second;
    --df; // pointing to Ti
    unsigned t1 = df->first;
    double rt1 = df->second;
    double r = (rt2 - rt1) / (t2 - t1);
    return std::exp(-rt1 - r * (day_diff - t1));

}

} // namespace minirisk
