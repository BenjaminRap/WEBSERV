#!/bin/bash

cd ../../code
make re
timeout 5s valgrind ./WebServ ../tests/configurations/oneCommentLine.txt &> WebServ.log
if [ $? -eq 0 ]
then
	echo OK
else
	echo KO
fi