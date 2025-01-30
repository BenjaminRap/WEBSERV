#!/bin/bash

runTest()
{
	echo test: $1
	(cd ../code && make -s re MAIN=../tests/scripts/$1 && valgrind --leak-check=full --show-leak-kinds=all ./WebServ)
}

runTest flowBufferTests.cpp
runTest socketDataTests.cpp
runTest socketDataTests.cpp
runTest socketsHandlerTests.cpp