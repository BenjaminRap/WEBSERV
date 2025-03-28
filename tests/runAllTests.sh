#!/bin/bash

runTest()
{
	echo test: $1
	(cd ../code && make -s re MAIN=../tests/scripts/$1 && valgrind --leak-check=full --show-leak-kinds=all ./WebServ ../code/configurations/fakenginx.conf)
}

runTest flowBufferTests.cpp
runTest getLineTests.cpp
runTest aFdDataTests.cpp
runTest socketsHandlerTests.cpp
