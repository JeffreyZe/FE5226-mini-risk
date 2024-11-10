#include "Market.h"
#include "CurveDiscount.h"

#include <vector>
#include <limits>

namespace minirisk {

template <typename I, typename T>
std::shared_ptr<const I> Market::get_curve(const string& name)
{
    ptr_curve_t& curve_ptr = m_curves[name];
    if (!curve_ptr.get())
        curve_ptr.reset(new T(this, m_today, name));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(curve_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

const ptr_disc_curve_t Market::get_discount_curve(const string& name)
{
    return get_curve<ICurveDiscount, CurveDiscount>(name);
}

double Market::from_mds(const string& objtype, const string& name)
{
    auto ins = m_risk_factors.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        ins.first->second = m_mds->get(name);
    }
    return ins.first->second;
}

unsigned Market::to_days(const string& key, const string& ccyname)
{
    unsigned unit=0, period=0, extra_length;
    extra_length = ir_rate_prefix.length() + ccyname.length() + 1; // 1 is for the '.' before ccy
    string middle_part = key.substr(ir_rate_prefix.length(), key.length()-extra_length); // keep as '10W'
    char c = middle_part.back(); // 'W'
    middle_part.pop_back();
    unit = std::stoi(middle_part); // 10
    switch (c)
    {
    case 'D':
        period = 1;
        break;
    case 'W':
        period = 7;
        break;
    case 'M':
        period = 30;
        break;
    case 'Y':
        period = 365;
        break;
    default:
        MYASSERT(false, "Please input D/W/M/Y to get the yield curve.");
        break;
    }

    return unit * period;
}

const std::map<unsigned, double> Market::get_yield(const string& ccyname)
{
    string expr = ir_rate_prefix + "\\d+[DWMY]." + ccyname;
    std::vector<std::string> matched_keys = m_mds->match(expr);
    std::map<unsigned, double> yield_curve;
    unsigned t;
    for (const string& key : matched_keys)
    {
        t = to_days(key, ccyname);
        yield_curve.emplace(t, t * from_mds("yield curve", key)/365.0);
    }
    return yield_curve;
};

const double Market::get_fx_spot(const string& name)
{
    return from_mds("fx spot", mds_spot_name(name));
}

void Market::set_risk_factors(const vec_risk_factor_t& risk_factors)
{
    clear();
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
    }
}

Market::vec_risk_factor_t Market::get_risk_factors(const std::string& expr) const
{
    vec_risk_factor_t result;
    std::regex r(expr);
    for (const auto& d : m_risk_factors)
        if (std::regex_match(d.first, r))
            result.push_back(d);
    return result;
}

} // namespace minirisk
