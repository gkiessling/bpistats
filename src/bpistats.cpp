#include <iostream>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "stat_printer.hpp"
#include "stat_aggregator.hpp"

using namespace boost::program_options;
using namespace boost::property_tree;

void print_usage(const options_description& desc)
{
    cout << "Usage: bpistats [OPTION]... FILE" << endl;
    cout << "Generates a report summarizing Bitcoin prices over a date range" << endl;
    cout << "provided in the JSON file FILE. The file must have a top-level \"bpi\"" << endl;
    cout << "key whose value is an associative array of date keys and floating-" << endl;
    cout << "point values." << endl;
    cout << desc << endl;
}

int main(int argc, char *argv[])
{
    ptree data_in;

    options_description desc("Options");
    desc.add_options()
        ("help,h", "Display help message");
    variables_map vm;

    try
    {
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
    }
    catch (exception e)
    {
        print_usage(desc);
        return 1;
    }    

    if (argc < 2 || vm.count("help"))
    {
        print_usage(desc);
        return 1;
    }

    try
    {
        read_json(argv[1], data_in);
    }
    catch (json_parser_error e)
    {
        cerr << "Error reading json file " << argv[1] <<  ": " << e.message() << endl;
        return 1;
    }

    auto stats(data_in.get_child_optional("bpi"));

    if (!stats)
    {
        cerr << "No \"bpi\" key in input data." << endl;
        return 1;
    }

    print_stat(cout, "Highest", aggregate<ptree_highest>(stats->begin(), stats->end()));
    print_stat(cout, "Lowest ", aggregate<ptree_lowest>(stats->begin(), stats->end()));
    print_stat(cout, "Count  ", aggregate<ptree_count>(stats->begin(), stats->end()));
    print_stat(cout, "Average", aggregate<ptree_average>(stats->begin(), stats->end()));
    print_stat(cout, "Median ", aggregate<ptree_median>(stats->begin(), stats->end()));
    print_stat(cout, "Std Dev", aggregate<ptree_stddev>(stats->begin(), stats->end()));

    return 0;
}
