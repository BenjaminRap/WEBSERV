createRandomFile()
{
    local path="$1"

	# dd if=/dev/urandom of="$path" bs=1 count=20 &>/dev/null
	touch $path
	echo "truc" >> $path
}

# Create Working Directory
mkdir get
chmod 777 post/
createRandomFile main.html

##---------------------------------------------------------------------------------------------------##
# Test 1 : Classic case
	createRandomFile post/main.html

# Test 2 : Classic Case (file in folder)
	mkdir post/fake/
	createRandomFile post/fake/main.cpp

# Test 3, 4, 5: Classic but with '../' in url
	createRandomFile post/fake/main.html

##---------------------------------------------------------------------------------------------------##
# Test 6 : Index Case
	createRandomFile post/index.html
# Test 6 Index case (but not index.html)
	mkdir post/srcs/
	createRandomFile post/srcs/main.cpp

##---------------------------------------------------------------------------------------------------##
# Test 8 : No perms folder
	mkdir post/nonono/
	chmod 000 post/nonono/

##---------------------------------------------------------------------------------------------------##
# Test 9 : 405 Meth Not Allow
	mkdir post/405/

##---------------------------------------------------------------------------------------------------##
# Test 10 : AutoIndex
	mkdir post/auto/
	createRandomFile post/auto/main.html
	createRandomFile post/auto/main.cpp
	createRandomFile post/auto/main.js
# Test 11 : AutoIndex with many file and folder
	mkdir post/auto2/
	createRandomFile post/auto2/main.html
	createRandomFile post/auto2/main.cpp
	createRandomFile post/auto2/main.js
	mkdir post/auto2/fake/
	createRandomFile post/auto2/fake/main.html
	createRandomFile post/auto2/fake/main.cpp
	createRandomFile post/auto2/fake/main.js
# Test 11 : url to file with backlash
	createRandomFile post/truc.txt
# Test 12 : index is folder
	mkdir -p post/indexIsFolder/index
# Test 13: index is symlink
	mkdir post/indexIsSymlink
	createRandomFile post/indexIsSymlink/truc
	ln -s $(pwd)/post/indexIsSymlink/truc $(pwd)/post/indexIsSymlink/sym

# Test 14: two index : folder and file
	mkdir -p post/indexIsFolderAndFile/main
	createRandomFile post/indexIsFolderAndFile/main.html

# Test 15: two index : symlink and file
	mkdir -p post/indexIsSymlinkAndFile/
	createRandomFile post/indexIsSymlinkAndFile/main.html
	ln -s $(pwd)/post/indexIsSymlinkAndFile/main.html $(pwd)/post/indexIsSymlinkAndFile/sym

#Test 16: folder that ends with '.'
	mkdir post/endWith./
	createRandomFile post/endWith./index.html
