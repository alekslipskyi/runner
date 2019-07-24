#ifndef CONSTANTS
#define CONSTANTS

#include <string>

using namespace std;

namespace errors {
    namespace CLI {
        extern const char *NO_COMMAND;
        extern const char *NO_CONFIG;
        extern const char *UNKNOWN_COMMAND;
    }

    namespace CONFIG {
        extern const char *NO_COMMAND;
    }
}

namespace commandInterface {
    namespace FLAGS {
        extern const char *WATCH_PATH;
        extern const char *WATCH_FILE;
        extern const char *PORT;
        extern const char *EXEC;
        extern const char *CONFIG;
        extern const char *FOREGROUND;
    }

    namespace COMMANDS {
        extern const char *RUN;
        extern const char *LIST;
    }

    namespace OUTPUT {
        extern const int TABS;
    }
}

namespace configInterface {
    namespace CONFIG {
        extern const char *WATCH_PATH;
        extern const char *WATCH_FILE;
        extern const char *PORT;
        extern const char *EXEC;
        extern const char *CONFIG;
        extern const char *FOREGROUND;
    }
}

namespace CORE {
    namespace TMP {
        extern const char *PUB_FILE;
    }

    extern const char *DEFAULT_VALUE_ENV;
}

class ENV {
public:
    string      command;
    bool        isForeground;
    string      path_to_config;
    string      path_to_watch;
    string      path_to_watch_file;
    string      port;
};

#endif