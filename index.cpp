#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "src/cli/index.h"
#include "src/constants/index.h"
#include "src/core/index.h"

using namespace std;

int main(int argc, char *argv[]) {
    CLI cli(argc, argv);
    ENV env = cli.getEnv();
    Core core(&env);

    if (!strlen(env.command.c_str())) return 1;

    if (env.isForeground) {
        core.exec();
    } else {
        core.putToBackground(argc, argv);
    }

    return 1;
}