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
            "{\"2018-01-01\": 222.22,"
            " \"2018-01-02\": 111.11,"
            " \"2018-01-03\": 444.44,"
            " \"2018-01-04\": 333.33}";
        read_json(stream, test_tree);
    }

    ~ptree_fixture() {}

    ptree test_tree;
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
    BOOST_CHECK_EQUAL(1111.1, result.second);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(test_get_highest, ptree_fixture)

BOOST_AUTO_TEST_CASE(get_highest_returns_highest)
{
    pair<string, double> result = aggregate<ptree_highest>(test_tree.begin(), test_tree.end());
    BOOST_CHECK_EQUAL("2018-01-03", result.first);
    BOOST_CHECK_EQUAL(444.44, result.second);
}

BOOST_AUTO_TEST_SUITE_END()

