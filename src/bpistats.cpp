#include <iostream>
#include <boost/property_tree/json_parser.hpp>

//#include "stat_printer.hpp"
#include "stat_aggregator.hpp"

using namespace boost::property_tree;

int main(int argc, char *argv[])
{
    ptree data_in;

    if (!argc)
    {
    //     usage and exit
    }

    try
    {
        read_json(argv[0], data_in);
    }
    catch (json_parser_error e)
    {
    // usage and exit
    }

//    print_stat<get_highest>(STDOUT, data_in);
//    print_stat<get_lowest>(STDOUT, data_in);

    return 0;
}
