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
        const char *ENV_FILE = "--env";
    }

    namespace COMMANDS {
        const char *RUN = "run";
        const char *LIST = "list";
    }

    namespace COMMANDS_FOREGROUND {
        const char *STOP = "stop";
        const char *RESUME = "resume";
        const char *RELOAD = "reload";
    }

    namespace OUTPUT {
        const int TABS = 4;
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

namespace CORE {
    namespace TMP {
        const char *PUB_FILE = "/tmp/runner_tasks.tmp";
        const char *LSOF_TMP = "/tmp/lsof_tmp.out";
    }

    const char *DEFAULT_VALUE_ENV = "none";
    const char *APP_NAME = "runner";
}

bool test_const() {
    return true;
}