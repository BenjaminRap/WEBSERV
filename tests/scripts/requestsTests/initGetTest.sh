# Create Working Directory
mkdir get
chmod 777 get/
touch main.html

##---------------------------------------------------------------------------------------------------##
# Test 1 : Classic case
	touch get/main.html

# Test 2 : Classic Case (file in folder)
	mkdir get/fake/
	touch get/fake/main.cpp

# Test 3, 4, 5: Classic but with '../' in url
	touch get/fake/main.html

##---------------------------------------------------------------------------------------------------##
# Test 6 : Index Case
	touch get/index.html
# Test 6 Index case (but not index.html)
	mkdir get/srcs/
	touch get/srcs/main.cpp

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
	touch get/auto/main.html
	touch get/auto/main.cpp
	touch get/auto/main.js
# Test 11 : AutoIndex with many file and folder
	mkdir get/auto2/
	touch get/auto2/main.html
	touch get/auto2/main.cpp
	touch get/auto2/main.js
	mkdir get/auto2/fake/
	touch get/auto2/fake/main.html
	touch get/auto2/fake/main.cpp
	touch get/auto2/fake/main.js
# Test 11 : url to file with backlash
	touch get/truc.txt
# Test 12 : index is folder
	mkdir -p get/indexIsFolder/index
# Test 13: index is symlink
	mkdir get/indexIsSymlink
	touch get/indexIsSymlink/truc
	ln -s $(pwd)/get/indexIsSymlink/truc $(pwd)/get/indexIsSymlink/sym

# Test 14: two index : folder and file
	mkdir -p get/indexIsFolderAndFile/main
	touch get/indexIsFolderAndFile/main.html

# Test 15: two index : symlink and file
	mkdir -p get/indexIsSymlinkAndFile/
	touch get/indexIsSymlinkAndFile/main.html
	ln -s $(pwd)/get/indexIsSymlinkAndFile/main.html $(pwd)/get/indexIsSymlinkAndFile/sym

#Test 16: folder that ends with '.'
	mkdir get/endWith./
	touch get/endWith./index.html
