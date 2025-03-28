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

runIndependentTestFromFolder()
{
	echo test: $1
	(cd $2 && c++ $1 -o test && ./test && rm test)
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
}

# runTestChangingMain flowBufferTests.cpp
# runTestChangingMain getLineTests.cpp
# runTestChangingMain aFdDataTests.cpp
# runTestChangingMain socketsHandlerTests.cpp
runWebServInBackground
PID=$!
trap "kill -2 $PID 2>/dev/null" EXIT
runNginxInBackground
PID=$!
trap "kill -2 $PID 2>/dev/null" EXIT
runIndependentTestFromFolder getTests.cpp scripts/requestsTests
runIndependentTestFromFolder putTests.cpp scripts/requestsTests
runIndependentTestFromFolder deleteTests.cpp scripts/requestsTests
docker container ls | awk '{print $1}' | tail -n +2 | xargs docker container stop
