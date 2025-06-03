createRandomFile()
{
    local path="$1"

	dd if=/dev/urandom of="$path" bs=1 count=20 status=none
}

mkdir put
chmod 777 put/

##-----------------------------------------------------------------------------------------------------##
# test 1 : Classic case
	mkdir put/allowed/
	chmod 777 put/allowed
# test 2 : Method Not Allowed
	mkdir put/notAllowed
	chmod 777 put/notAllowed
# test 3 : Forbidden
	mkdir put/forbidden
	chmod 000 put/forbidden
# test 4 : Already existing with right
	mkdir put/alreadyExisting
  	createRandomFile put/alreadyExisting/mainRight.html
# test5 : Already existing with no right
	createRandomFile put/alreadyExisting/mainNoRight.html
	chmod 000 put/alreadyExisting/mainNoRight.html
# test6 : Creating a directory that already exists with right and empty
	mkdir put/alreadyExistingDirEmpty
# test7 : Creating a directory that already exists with right and not empty
	mkdir put/alreadyExistingDir
	createRandomFile put/alreadyExistingDir/dummy.txt
# test8 : Creating a directory that already exists with no right and empty
	mkdir put/alreadyExistingDirEmptyNoRight
	chmod 000 put/alreadyExistingDirEmptyNoRight
# test9 : Creating a directory that already exists with no right and not empty
	mkdir put/alreadyExistingDirNoRight
	createRandomFile put/alreadyExistingDirNoRight/dummy.txt
	chmod 000 put/alreadyExistingDirNoRight/dummy.txt
