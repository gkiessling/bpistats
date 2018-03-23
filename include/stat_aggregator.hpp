#ifndef _STAT_AGGREGATOR_HPP_
#define _STAT_AGGREGATOR_HPP_

#include <utility>

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost::property_tree;

// -----------------------------------------------------------------------------
// aggregate
// Aggregates a range of values (or selects a single value) using an aggregation
// function object. At the end of aggregation, calls the get_result() method to
// retrieve the result.
// -----------------------------------------------------------------------------
template <class Aggregator, class InputIterator>
pair<string, double> aggregate(InputIterator first, InputIterator last)
{
    Aggregator aggregator;
    for_each(first, last, ref(aggregator));
    return aggregator.get_result();
}

// -----------------------------------------------------------------------------
// class ptree_selector
// A base function object that selects a single string/double pair from the top-
// level of a provided property tree. The pair selected is the last one for
// which the comparator returns true. Sub-classes may override the get_result()
// method to perform a final operation on the selected pair. Pairs whose values
// cannot be converted to double are skipped.
// -----------------------------------------------------------------------------
class ptree_selector
{
public:
    ptree_selector(function<bool(double, double)> bool_comparator)
        : comparator(bool_comparator) {}

    ptree_selector()
        : comparator([](double, double) { return false; }) {}

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

    virtual pair<string, double> get_result()
    {
        return selected;
    }

protected:
    pair<string, double> selected;
    function<bool(double, double)> comparator;
};

// -----------------------------------------------------------------------------
// class ptree_highest
// A ptree_selector that selects the highest value in the provided property
// tree.
// -----------------------------------------------------------------------------
class ptree_highest : public ptree_selector
{
public:
    ptree_highest() : ptree_selector([](double new_val, double cur_val) { return new_val > cur_val; }) {}
};

// -----------------------------------------------------------------------------
// class ptree_lowest
// A ptree_selector that selects the highest value in the provided property
// tree.
// -----------------------------------------------------------------------------
class ptree_lowest : public ptree_selector
{
public:
    ptree_lowest() : ptree_selector([](double new_val, double cur_val) { return !cur_val || new_val < cur_val; }) {}
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class ptree_median : public ptree_selector
{
public:
    ptree_median() : ptree_selector([this](double new_val, double cur_val)
        {
            if (!cur_val)
            {
                return true;
            }

            // If other_val is not set, save both values; median will be the
            // average of the two. Save cur_val (and current selected pair)
            // and return true so that new_val becomes current.
            if (!other_val)
            {
                other_val = cur_val;
                other_pair = selected;
                return true;
            }
          
            // Otherwise, figure out which is the new median.
            double lower, higher;
            pair<string, double> lower_pair, higher_pair;

            if (cur_val < other_val)
            {
                lower = cur_val;
                lower_pair = selected;
                higher = other_val;
                higher_pair = other_pair;
            }
            else
            {
                lower = other_val;
                lower_pair = other_pair;
                higher = cur_val;
                higher_pair = selected;
            }

            if (new_val < lower)
            {
                selected = lower_pair;
                other_val = 0.0;
                return false;
            }
            else if (new_val > higher)
            {
                selected = higher_pair;
                other_val = 0.0;
                return false;
            }
            else
            {
                // new_val is the new median
                return true;
            }
        }) {}

    pair<string, double> get_result() override
    {
        if (other_val)
        {
            return pair<string, double>("", (selected.second + other_val) / 2);
        }
        else
        {
            return selected;
        }
    }

private:
    double other_val = 0.0;
    pair<string, double> other_pair;
};

// -----------------------------------------------------------------------------
// class ptree_selector
// A base function object that aggregates the values of string/double pairs from
// the top-level of a provided property tree. Sub-classes may override the
// get_result() method to perform a final operation on the aggregation. Pairs
// whose values cannot be converted to double are skipped.
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// class ptree_count
// A ptree_aggregator that counts the number of pairs in the property tree.
// -----------------------------------------------------------------------------
class ptree_count : public ptree_aggregator<size_t>
{
public:
    ptree_count() : ptree_aggregator([](size_t& count, double val) { ++count; }) {}
};

// -----------------------------------------------------------------------------
// class ptree_sum
// A ptree_aggregator that sums the values from the property tree.
// -----------------------------------------------------------------------------
class ptree_sum : public ptree_aggregator<double>
{
public:
    ptree_sum() : ptree_aggregator([this](double& sum, double val) { ++count; sum += val; }) {}

protected:
    size_t count = 0;
};

// -----------------------------------------------------------------------------
// class ptree_average
// A ptree_aggregator that calculates the average of values from the property
// tree.
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class ptree_stddev : public ptree_selector {};

#endif
