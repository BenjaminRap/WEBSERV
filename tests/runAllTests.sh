#!/bin/bash

runTestChangingMain()
{
	echo test: $1
	cd ../code
	make fclean
	make -s -j16 MAIN=../tests/scripts/$1
	valgrind --leak-check=full --show-leak-kinds=all ./WebServ ./configurations/fakenginx.conf
	cd ../tests
}

runRequestTest()
{
	echo test: $1
	(cd scripts/requestsTests/ && node $1)
}

runWebServInBackground()
{
	echo running Webserv
	cd ../code
	make fclean
	make -s -j16
	valgrind --leak-check=full --show-leak-kinds=all ./WebServ ./configurations/fakenginx.conf &> /dev/null &
	cd ../tests
}

runNginxInBackground()
{
	echo running nginx
	make all &> /dev/null &
	sleep 5
	echo sleeping 5 seconds to wait for nginx to start
}

runTestChangingMain flowBufferTests.cpp
runTestChangingMain getLineTests.cpp
runTestChangingMain aFdDataTests.cpp
runTestChangingMain socketsHandlerTests.cpp
runWebServInBackground
pidWebServ=$!
runNginxInBackground
pidNginx=$!
trap "kill -2 $pidNginx 2>/dev/null && kill -2 $pidWebServ 2>/dev/null" EXIT
runRequestTest getTests.mjs
runRequestTest putTests.mjs
runRequestTest deleteTests.mjs
runRequestTest parsingTests.mjs
docker container ls | awk '{print $1}' | tail -n +2 | xargs docker container stop
