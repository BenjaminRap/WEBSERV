createRandomFile()
{
    local path="$1"

	dd if=/dev/urandom of="$path" bs=1 count=20 &>/dev/null
}

# Create Working Directory
mkdir get
chmod 777 get/
createRandomFile main.html

##---------------------------------------------------------------------------------------------------##
# Test 1 : Classic case
	createRandomFile get/main.html

# Test 2 : Classic Case (file in folder)
	mkdir get/fake/
	createRandomFile get/fake/main.cpp

# Test 3, 4, 5: Classic but with '../' in url
	createRandomFile get/fake/main.html

##---------------------------------------------------------------------------------------------------##
# Test 6 : Index Case
	createRandomFile get/index.html
# Test 6 Index case (but not index.html)
	mkdir get/srcs/
	createRandomFile get/srcs/main.cpp

##---------------------------------------------------------------------------------------------------##
# Test 8 : No perms folder
	mkdir get/nonono/
	chmod 000 get/nonono/

##---------------------------------------------------------------------------------------------------##
# Test 9 : 405 Meth Not Allow
	mkdir get/405/

##---------------------------------------------------------------------------------------------------##
# Test 10 : AutoIndex
	mkdir get/auto/
	createRandomFile get/auto/main.html
	createRandomFile get/auto/main.cpp
	createRandomFile get/auto/main.js
# Test 11 : AutoIndex with many file and folder
	mkdir get/auto2/
	createRandomFile get/auto2/main.html
	createRandomFile get/auto2/main.cpp
	createRandomFile get/auto2/main.js
	mkdir get/auto2/fake/
	createRandomFile get/auto2/fake/main.html
	createRandomFile get/auto2/fake/main.cpp
	createRandomFile get/auto2/fake/main.js
# Test 11 : url to file with backlash
	createRandomFile get/truc.txt
# Test 12 : index is folder
	mkdir -p get/indexIsFolder/index
# Test 13: index is symlink
	mkdir get/indexIsSymlink
	createRandomFile get/indexIsSymlink/truc
	ln -s $(pwd)/get/indexIsSymlink/truc $(pwd)/get/indexIsSymlink/sym

# Test 14: two index : folder and file
	mkdir -p get/indexIsFolderAndFile/main
	createRandomFile get/indexIsFolderAndFile/main.html

# Test 15: two index : symlink and file
	mkdir -p get/indexIsSymlinkAndFile/
	createRandomFile get/indexIsSymlinkAndFile/main.html
	ln -s $(pwd)/get/indexIsSymlinkAndFile/main.html $(pwd)/get/indexIsSymlinkAndFile/sym

#Test 16: folder that ends with '.'
	mkdir get/endWith./
	createRandomFile get/endWith./index.html
