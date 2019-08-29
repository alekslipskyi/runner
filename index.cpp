#include <cstring>

#include "src/cli/index.h"

using namespace std;

int main(int argc, char *argv[]) {
    ENV env = CLI::getEnv(argc, argv);
    CLI cli(env, argc, argv);
    if (!strlen(env.command.c_str())) return 1;
    return 1;
}