#ifndef _STAT_PRINTER_HPP_
#define _STAT_PRINTER_HPP_

#include <iostream>

using namespace std;

void print_stat(ostream& stream, const string& heading, const pair<string, double>& stat)
{
    stream << heading << ": "
        << (stat.first.empty() ? "           " : stat.first + " ")
        << stat.second << endl;
}

#endif
