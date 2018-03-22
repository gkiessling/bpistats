#define BOOST_TEST_MODULE stat_selector_test
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <utility>

#include <boost/property_tree/json_parser.hpp>

#include "stat_selector.hpp"

using namespace std;
using namespace boost::property_tree;
using namespace stat_selector;

struct shared_fixture
{
    shared_fixture()
    {
        stringstream stream;
        stream <<
            "{\"2018-01-01\": 222.22,"
            " \"2018-01-02\": 111.11,"
            " \"2018-01-03\": 444.44,"
            " \"2018-01-04\": 333.33}";
        read_json(stream, test_tree);
    }

    ~shared_fixture() {}

    ptree test_tree;
};

BOOST_FIXTURE_TEST_SUITE(test_get_highest, shared_fixture)

BOOST_AUTO_TEST_CASE(get_highest_returns_highest)
{
    pair<string, double> result = get_highest(test_tree);
    BOOST_CHECK_EQUAL("2018-01-04", result.first);
    BOOST_CHECK_EQUAL(333.33, result.second);
}

BOOST_AUTO_TEST_SUITE_END()

