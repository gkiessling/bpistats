#define BOOST_TEST_MODULE stat_selector_test
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <utility>

#include <boost/property_tree/json_parser.hpp>

#include "stat_aggregator.hpp"

using namespace std;
using namespace boost::property_tree;

struct ptree_fixture
{
    ptree_fixture()
    {
        stringstream stream;
        stream <<
            "{\"2018-01-01\": 222,"
            " \"2018-01-02\": 111.11,"
            " \"2018-01-03\": 444.44,"
            " \"2018-01-04\": 333.33}";
        read_json(stream, test_tree);
        stream.clear();
        stream <<
            "{\"2018-01-01\": 222,"
            " \"2018-01-02\": \"abc\","
            " \"2018-01-03\": 111.11}";
        read_json(stream, invalid_tree);
        stream.clear();
        stream <<
            "{\"2018-01-01\": 111,"
            " \"2018-01-02\": 222.22,"
            " \"2018-01-03\": 444.44,"
            " \"2018-01-04\": 333.33,"
            " \"2018-01-05\": 555.55}";
        read_json(stream, odd_length_tree);
    }

    ~ptree_fixture() {}

    ptree test_tree;
    ptree empty_tree;
    ptree invalid_tree;
    ptree odd_length_tree;
};

struct test_aggregator
{
    void operator()(const pair<string, ptree>& stats_tree)
    {
        dates += stats_tree.first;
        sum += stats_tree.second.get<double>("");
    }

    pair<string, double> get_result()
    {
        return pair<string, double>(dates, sum);
    }

    double sum = 0.0;
    string dates;
};


BOOST_FIXTURE_TEST_SUITE(test_aggregate, ptree_fixture)

BOOST_AUTO_TEST_CASE(aggregate_calls_aggregator)
{
    pair<string, double> result = aggregate<test_aggregator>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("2018-01-012018-01-022018-01-032018-01-04", result.first);
    BOOST_CHECK_EQUAL(111088, round(result.second*100.0));
}

BOOST_AUTO_TEST_CASE(aggregate_empty_tree_does_not_call_aggregator)
{
    pair<string, double> result = aggregate<test_aggregator>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0.0, result.second);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(test_aggregators, ptree_fixture)

BOOST_AUTO_TEST_CASE(get_highest_returns_highest)
{
    pair<string, double> result = aggregate<ptree_highest>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("2018-01-03", result.first);
    BOOST_CHECK_EQUAL(444.44, result.second);
}

BOOST_AUTO_TEST_CASE(get_highest_skips_invalid)
{
    pair<string, double> result = aggregate<ptree_highest>(invalid_tree.begin(), invalid_tree.end());
    BOOST_CHECK_EQUAL("2018-01-01", result.first);
    BOOST_CHECK_EQUAL(222.0, result.second);
}

BOOST_AUTO_TEST_CASE(get_highest_empty_returns_zero)
{
    pair<string, double> result = aggregate<ptree_highest>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0.0, result.second);
}

BOOST_AUTO_TEST_CASE(get_lowest_returns_lowest)
{
    pair<string, double> result = aggregate<ptree_lowest>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("2018-01-02", result.first);
    BOOST_CHECK_EQUAL(111.11, result.second);
}

BOOST_AUTO_TEST_CASE(get_lowest_skips_invalid)
{
    pair<string, double> result = aggregate<ptree_lowest>(invalid_tree.begin(), invalid_tree.end());
    BOOST_CHECK_EQUAL("2018-01-03", result.first);
    BOOST_CHECK_EQUAL(111.11, result.second);
}

BOOST_AUTO_TEST_CASE(get_lowest_empty_returns_zero)
{
    pair<string, double> result = aggregate<ptree_highest>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0.0, result.second);
}

BOOST_AUTO_TEST_CASE(count_returns_count)
{
    pair<string, double> result = aggregate<ptree_count>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(4, result.second);
}

BOOST_AUTO_TEST_CASE(count_skips_invalid)
{
    pair<string, double> result = aggregate<ptree_count>(invalid_tree.begin(), invalid_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(2, result.second);
}

BOOST_AUTO_TEST_CASE(count_empty_returns_zero)
{
    pair<string, double> result = aggregate<ptree_count>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0, result.second);
}

BOOST_AUTO_TEST_CASE(get_average_returns_average)
{
    pair<string, double> result = aggregate<ptree_average>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(27772, round(result.second*100.0));
}

BOOST_AUTO_TEST_CASE(get_average_skips_invalid)
{
    pair<string, double> result = aggregate<ptree_average>(invalid_tree.begin(), invalid_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(166555, round(result.second*1000.0));
}

BOOST_AUTO_TEST_CASE(get_average_empty_returns_zero)
{
    pair<string, double> result = aggregate<ptree_average>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0.0, result.second);
}

BOOST_AUTO_TEST_CASE(get_median_even_returns_no_date)
{
    pair<string, double> result = aggregate<ptree_median>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(277665, round(result.second*1000.0));
}

BOOST_AUTO_TEST_CASE(get_median_odd_returns_date)
{
    pair<string, double> result = aggregate<ptree_median>(odd_length_tree.begin(), odd_length_tree.end());
    BOOST_CHECK_EQUAL("2018-01-04", result.first);
    BOOST_CHECK_EQUAL(333.33, result.second);
}

BOOST_AUTO_TEST_CASE(get_median_skips_invalid)
{
    pair<string, double> result = aggregate<ptree_median>(invalid_tree.begin(), invalid_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(166555, round(result.second*1000.0));
}

BOOST_AUTO_TEST_CASE(get_median_empty_returns_zero)
{
    pair<string, double> result = aggregate<ptree_median>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0.0, result.second);
}

#if 0
BOOST_AUTO_TEST_CASE(get_stddev_returns_stddev)
{
    pair<string, double> result = aggregate<ptree_stddev>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_GT(143.47, result.second);
    BOOST_CHECK_LT(143.48, result.second);
}

BOOST_AUTO_TEST_CASE(get_stddev_skips_invalid)
{
    pair<string, double> result = aggregate<ptree_stddev>(invalid_tree.begin(), invalid_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_GT(78.41, result.second);
    BOOST_CHECK_LT(78.42, result.second);
}

BOOST_AUTO_TEST_CASE(get_stddev_empty_returns_zero)
{
    pair<string, double> result = aggregate<ptree_stddev>(empty_tree.begin(), empty_tree.end());
    BOOST_CHECK_EQUAL("", result.first);
    BOOST_CHECK_EQUAL(0.0, result.second);
}
#endif

BOOST_AUTO_TEST_SUITE_END()

