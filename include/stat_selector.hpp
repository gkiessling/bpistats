#ifndef _STAT_SELECTOR_HPP_
#define _STAT_SELECTOR_HPP_

#include <utility>

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost::property_tree;

namespace stat_selector
{

pair<string, double> get_highest(const ptree& stats_tree)
{
    return pair<string, double>();
}

} // end namespace stat_selector
#endif
