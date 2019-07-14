#include <iostream>
#include <iomanip>
#include <vector>

#include "index.h"

void drawTableHeader() {
    cout
    << setw(commandInterface::OUTPUT::TABS) << "ID |"
    << setw(commandInterface::OUTPUT::TABS) << " PID |"
    << setw(commandInterface::OUTPUT::TABS) << " commmand |"
    << setw(commandInterface::OUTPUT::TABS) << " watch path |"
    << setw(commandInterface::OUTPUT::TABS) << " config |"
    << setw(commandInterface::OUTPUT::TABS) << " watch file |"
    << setw(commandInterface::OUTPUT::TABS) << " port" << endl;

    cout
    << setw(commandInterface::OUTPUT::TABS) << "___|"
    << setw(commandInterface::OUTPUT::TABS) << "_____|"
    << setw(commandInterface::OUTPUT::TABS) << "__________|"
    << setw(commandInterface::OUTPUT::TABS) << "____________|"
    << setw(commandInterface::OUTPUT::TABS) << "________|"
    << setw(commandInterface::OUTPUT::TABS) << "________|"
    << setw(commandInterface::OUTPUT::TABS) << "_____" << endl;
}

void drawTableBody(vector<string> splitted) {
    cout << "here body" << splitted[5] << endl;

    cout
    << setw(commandInterface::OUTPUT::TABS) << splitted[0] << "|"
    << setw(commandInterface::OUTPUT::TABS) << splitted[1] << "|"
    << setw(commandInterface::OUTPUT::TABS) << splitted[2] << "|"
    << setw(commandInterface::OUTPUT::TABS) << splitted[3] << "|"
    << setw(commandInterface::OUTPUT::TABS) << splitted[4] << "|"
    << setw(commandInterface::OUTPUT::TABS) << splitted[5] << "|"
    << setw(commandInterface::OUTPUT::TABS) << splitted[6] << endl;
}