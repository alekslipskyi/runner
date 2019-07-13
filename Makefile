compiler=g++
cversion=-std=c++17
flags=-ljsoncpp -lstdc++fs

INSTALLDIR=/usr/bin

glob=$(INSTALLDIR)/runner
glob_test=$(INSTALLDIR)/runner_test
local=./runner

build=$(compiler) $(cversion)

sources=$(shell find src/ -type f -name *.cpp)
headers=$(shell find src -type f -name *.h)
index_source=index.cpp

build_command=$(build) $(headers) $(sources) $(index_source)

glob_test:	$(header) $(sources)
			$(build_command) -o $(glob_test) $(flags)

glob:		$(header) $(sources)
			$(build_command) -o $(glob) $(flags)

local:		$(header) $(sources)
			$(build_command) -o $(local) $(flags)