#!/bin/bash

runTest()
{
	(cd ../code && make -s re MAIN=../tests/scripts/$1 && valgrind ./WebServ)
}

runTest flowBufferTests.cpp
runTest socketDataTests.cpp
runTest socketDataTests.cpp
runTest socketsHandlerTests.cpp