#ifndef _STAT_AGGREGATOR_HPP_
#define _STAT_AGGREGATOR_HPP_

#include <utility>

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost::property_tree;

template <class Aggregator, class InputIterator>
pair<string, double> aggregate(InputIterator first, InputIterator last)
{
    Aggregator aggregator;
    for_each(first, last, ref(aggregator));
    return aggregator.get_result();
}

class ptree_selector
{
public:
    ptree_selector(function<bool(double, double)> bool_comparator)
        : comparator(bool_comparator) {}

    ptree_selector()
        : comparator([](double, double){ return false; }) {}

    virtual ~ptree_selector() {}

    void operator()(const pair<string, ptree>& stats_tree)
    {
        auto val(stats_tree.second.get<double>(""));

        if (comparator(val, selected.second))
        {
            selected = pair<string, double>(stats_tree.first, val);
        }
    }

    pair<string, double> get_result()
    {
        return selected;
    }

private:
    pair<string, double> selected;
    function<bool(double, double)> comparator;
};

class ptree_highest : public ptree_selector
{
public:
    ptree_highest() : ptree_selector([](double new_val, double cur_val) { return new_val > cur_val; }) {}
};

class ptree_lowest : public ptree_selector {};
class ptree_count : public ptree_selector {};
class ptree_average : public ptree_selector {};
class ptree_median : public ptree_selector {};
class ptree_stddev : public ptree_selector {};

#endif
