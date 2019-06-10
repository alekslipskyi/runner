#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "./core/index.h"

using namespace std;

int main(int argc, char *argv[]) {
    Core core(argc, argv);

    core.exec();

    return 1;
}