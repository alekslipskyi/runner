#ifndef RUNNER_TABLE_H
#define RUNNER_TABLE_H

#include <vector>
#include <string>

#include "../constants/index.h"

using namespace std;

typedef struct TableWidth {
    int ID;
    int PID;
    int COMMAND;
    int WATCH_PATH;
    int WATCH_FILE;
    int CONFIG;
    int PORT;
} TableWidth;

enum TABLE_WIDTH_FIELDS { ID, PID, COMMAND, WATCH_PATH, WATCH_FILE, CONFIG, PORT };

class Table {
private:
    TableWidth              preventTableWidth;
    vector<string>          data;
    TableWidth              detectMaxWithForColumn();
public:
    void                    drawTableHeader();
    void                    drawTableBody();
    explicit Table(const vector<string> &data);
};


#endif
