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
        auto val(stats_tree.second.get_optional<double>(""));

        // Skip items that can't be converted to double
        if (val && comparator(*val, selected.second))
        {
            selected = pair<string, double>(stats_tree.first, *val);
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

class ptree_lowest : public ptree_selector
{
public:
    ptree_lowest() : ptree_selector([](double new_val, double cur_val) { return !cur_val || new_val < cur_val; }) {}
};

template<class AggregatorType>
class ptree_aggregator
{
public:
    ptree_aggregator(function<void(AggregatorType&, double)> aggregator)
        : aggregator(aggregator) {}

    virtual ~ptree_aggregator() {}

    void operator()(const pair<string, ptree>& stats_tree)
    {
        auto val(stats_tree.second.get_optional<double>(""));

        // Skip items that can't be converted to double
        if (val)
        {
            aggregator(aggregation, *val);
        }
    }

    virtual pair<string, double> get_result()
    {
        return pair<string, double>("", aggregation);
    }

protected:
    function<void(AggregatorType&, double)> aggregator;
    AggregatorType aggregation = {};
};

class ptree_count : public ptree_aggregator<size_t>
{
public:
    ptree_count() : ptree_aggregator([](size_t& count, double val) { ++count; }) {}
};

class ptree_sum : public ptree_aggregator<double>
{
public:
    ptree_sum() : ptree_aggregator([this](double& sum, double val) { ++count; sum += val; }) {}

protected:
    size_t count = 0;
};

class ptree_average : public ptree_sum
{
public:
    pair<string, double> get_result() override
    {
        if (count)
        {
            aggregation /= count;
        }

        return ptree_sum::get_result();
    }
};

class ptree_median : public ptree_selector {};
class ptree_stddev : public ptree_selector {};

#endif
