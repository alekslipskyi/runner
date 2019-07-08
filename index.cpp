#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "./core/index.h"
#include <signal.h>

using namespace std;

int main(int argc, char *argv[]) {
    Core core(argc, argv);

    if (core.isForegroundProcess()) {
        core.putToBackground(argc, argv);
    } else {
        core.exec();
    }

    return 1;
}