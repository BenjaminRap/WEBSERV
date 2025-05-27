# Create Working Directory
mkdir get
chmod 777 post/
touch main.html

##---------------------------------------------------------------------------------------------------##
# Test 1 : Classic case
	touch post/main.html

# Test 2 : Classic Case (file in folder)
	mkdir post/fake/
	touch post/fake/main.cpp

# Test 3, 4, 5: Classic but with '../' in url
	touch post/fake/main.html

##---------------------------------------------------------------------------------------------------##
# Test 6 : Index Case
	touch post/index.html
# Test 6 Index case (but not index.html)
	mkdir post/srcs/
	touch post/srcs/main.cpp

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
	touch post/auto/main.html
	touch post/auto/main.cpp
	touch post/auto/main.js
# Test 11 : AutoIndex with many file and folder
	mkdir post/auto2/
	touch post/auto2/main.html
	touch post/auto2/main.cpp
	touch post/auto2/main.js
	mkdir post/auto2/fake/
	touch post/auto2/fake/main.html
	touch post/auto2/fake/main.cpp
	touch post/auto2/fake/main.js
# Test 11 : url to file with backlash
	touch post/truc.txt
# Test 12 : index is folder
	mkdir -p post/indexIsFolder/index
# Test 13: index is symlink
	mkdir post/indexIsSymlink
	touch post/indexIsSymlink/truc
	ln -s $(pwd)/post/indexIsSymlink/truc $(pwd)/post/indexIsSymlink/sym

# Test 14: two index : folder and file
	mkdir -p post/indexIsFolderAndFile/main
	touch post/indexIsFolderAndFile/main.html

# Test 15: two index : symlink and file
	mkdir -p post/indexIsSymlinkAndFile/
	touch post/indexIsSymlinkAndFile/main.html
	ln -s $(pwd)/post/indexIsSymlinkAndFile/main.html $(pwd)/post/indexIsSymlinkAndFile/sym

#Test 16: folder that ends with '.'
	mkdir post/endWith./
	touch post/endWith./index.html
