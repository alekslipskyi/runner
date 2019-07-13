#include "index.h"

namespace errors {
    namespace CLI {
        const char *NO_COMMAND = R"(please specify the command, alike --exec="ls")";
        const char *NO_CONFIG = R"(config file are not specified like "runner run runner.json" or default config file runner.json is not exist)";
        const char *UNKNOWN_COMMAND = R"(are not supported)";
    }

    namespace CONFIG {
        const char *NO_COMMAND = R"(please add property exec, alike "exec":"ls")";
    }
}

namespace commandInterface {
    namespace FLAGS {
        const char *WATCH_PATH = "--watchpath";
        const char *WATCH_FILE = "--watchfile";
        const char *PORT = "--port";
        const char *EXEC = "--exec";
        const char *CONFIG = "--config";
        const char *FOREGROUND = "--foreground";
    }

    namespace COMMANDS {
        const char *RUN = "run";
    }
}

namespace configInterface {
    namespace CONFIG {
        const char *WATCH_PATH = "watchpath";
        const char *WATCH_FILE = "watchfile";
        const char *PORT = "port";
        const char *EXEC = "exec";
        const char *CONFIG = "config";
        const char *FOREGROUND = "foreground";
    }
}

bool test_const() {
    return true;
}