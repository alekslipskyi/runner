#include <iostream>
#include <iomanip>
#include <string.h>

#include "index.h"

Table::Table(const vector<string> &_data) {
    this->data = _data;
    this->preventTableWidth = TableWidth{};

    this->drawTableHeader();
    this->drawTableBody();
}

TableWidth Table::detectMaxWithForColumn() {
    TableWidth tableWidth{};
    int currentField = 0;

    for (auto &column: this->data) {
        int lengthOfColumn = strlen(column.c_str());

        switch (currentField) {
            case TABLE_WIDTH_FIELDS::ID: {
                if (!preventTableWidth.ID || (preventTableWidth.ID && preventTableWidth.ID < lengthOfColumn)) {
                    tableWidth.ID = lengthOfColumn;
                }
            }
            case TABLE_WIDTH_FIELDS::PID: {
                if (!preventTableWidth.PID || (preventTableWidth.PID && preventTableWidth.PID < lengthOfColumn)) {
                    tableWidth.PID = lengthOfColumn;
                }
            }
            case TABLE_WIDTH_FIELDS::COMMAND: {
                if (!preventTableWidth.COMMAND || (preventTableWidth.COMMAND && preventTableWidth.COMMAND < lengthOfColumn)) {
                    tableWidth.COMMAND = lengthOfColumn;
                }
            }
            case TABLE_WIDTH_FIELDS::WATCH_PATH: {
                if (!preventTableWidth.WATCH_PATH || (preventTableWidth.WATCH_PATH && preventTableWidth.WATCH_PATH < lengthOfColumn)) {
                    tableWidth.WATCH_PATH = lengthOfColumn;
                }
            }
            case TABLE_WIDTH_FIELDS::WATCH_FILE: {
                if (!preventTableWidth.WATCH_FILE || (preventTableWidth.WATCH_FILE && preventTableWidth.WATCH_FILE < lengthOfColumn)) {
                    tableWidth.WATCH_FILE = lengthOfColumn;
                }
            }
            case TABLE_WIDTH_FIELDS::CONFIG: {
                if (!preventTableWidth.CONFIG || (preventTableWidth.CONFIG && preventTableWidth.CONFIG < lengthOfColumn)) {
                    tableWidth.CONFIG = lengthOfColumn;
                }
            }
            case TABLE_WIDTH_FIELDS::PORT: {
                if (!preventTableWidth.PORT || (preventTableWidth.PORT && preventTableWidth.PORT < lengthOfColumn)) {
                    tableWidth.PORT = lengthOfColumn;
                }
            }
        }

        this->preventTableWidth = tableWidth;
        currentField++;
    }

    return tableWidth;
}

void Table::drawTableBody() {
    cout
    << setw(commandInterface::OUTPUT::TABS) << this->data[0] << "|"
    << setw(commandInterface::OUTPUT::TABS) << this->data[1] << "|"
    << setw(commandInterface::OUTPUT::TABS) << this->data[2] << "|"
    << setw(commandInterface::OUTPUT::TABS) << this->data[3] << "|"
    << setw(commandInterface::OUTPUT::TABS) << this->data[4] << "|"
    << setw(commandInterface::OUTPUT::TABS) << this->data[5] << "|"
    << setw(commandInterface::OUTPUT::TABS) << this->data[6] << endl;
}

void Table::drawTableHeader() {
    TableWidth tableWidth = this->detectMaxWithForColumn();

    cout
    << setw(tableWidth.ID + commandInterface::OUTPUT::TABS) << "ID |"
    << setw(tableWidth.PID + commandInterface::OUTPUT::TABS) << " PID |"
    << setw(tableWidth.COMMAND + commandInterface::OUTPUT::TABS) << " commmand |"
    << setw(tableWidth.WATCH_PATH + commandInterface::OUTPUT::TABS) << " watch path |"
    << setw(tableWidth.CONFIG + commandInterface::OUTPUT::TABS) << " config |"
    << setw(tableWidth.WATCH_FILE + commandInterface::OUTPUT::TABS) << " watch file |"
    << setw(tableWidth.PORT + commandInterface::OUTPUT::TABS) << " port" << endl;
}