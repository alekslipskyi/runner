glob:	filewatcher/index.h core/index.h core/index.cpp filewatcher/index.cpp index.cpp
		g++ -std=c++17 filewatcher/index.cpp core/index.cpp index.cpp -o /usr/bin/runner -ljsoncpp -lstdc++fs

local:	filewatcher/index.h core/index.h core/index.cpp filewatcher/index.cpp index.cpp
		g++ -std=c++17 filewatcher/index.cpp core/index.cpp index.cpp -o ./runner -ljsoncpp -lstdc++fs