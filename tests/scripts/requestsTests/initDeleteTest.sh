createRandomFile()
{
    local path="$1"

	# dd if=/dev/urandom of="$path" bs=1 count=20 &>/dev/null
	touch path
	echo "truc" >> path
}


# Create Working Directory
mkdir delete
chmod 777 delete/

##---------------------------------------------------------------------------------------------------##
	# Create Folder Full perms with 3 file.
	mkdir delete/full
	chmod 777 delete/full/

	# Test 1 : Classic : Full perms files, Full Perms Folder.
		createRandomFile delete/full/classic
		chmod 777 delete/full/classic
	# Test 2 : Classic with "../" in URL : Full perms files, Full perms folder.
		createRandomFile delete/full/noback
		chmod 777 delete/full/noback
	# Test 3 : Classic : No perms file, Full perms folder |
		createRandomFile delete/full/noperms
		chmod 000 delete/full/noperms

	# Create Folder with no perms with 1 file
	mkdir delete/cant


	# Test 4 : No Perms : Full perms Files, No Perms Folder
	#			Response for 'delete/cant/tryme' : 403 "Forbidden" | File NOT Delete
		createRandomFile delete/cant/tryme
		chmod 777 delete/cant/tryme
		chmod 000 delete/cant/

	# Create Folder with only perms to read with 1 file
	mkdir delete/readme
	# Test 5 : Full Permes FIles, Only read perms. |
	#			Response for '/delete/readme/deleteme' : 403 "Forbidden" | File NOT Delete
		createRandomFile delete/readme/deleteme
		chmod 777 delete/readme/deleteme
		chmod 444 delete/readme/

##---------------------------------------------------------------------------------------------------##
	# Create Working Directory
	mkdir delete/folder
	chmod 777 delete/folder/

	# Test 1 : Empty : Full perms parent, Full perms folder, Empty
		mkdir delete/folder/empty
		chmod 777 delete/folder/empty
	# Test 2 : Classic : Full perms parent, Full perms folder, Fill
		mkdir delete/folder/classic
		chmod 777 delete/folder/classic
		createRandomFile delete/folder/classic/file delete/folder/classic/file2 delete/folder/classic/file3 delete/folder/classic/file4
		chmod 777 delete/folder/classic/file delete/folder/classic/file2 delete/folder/classic/file3 delete/folder/classic/file4
	# Test 3 : No Perms parent : No perms parent, Full perms folder, Fill
		mkdir delete/folder/nopermspa
		mkdir delete/folder/nopermspa/dir
		createRandomFile delete/folder/nopermspa/dir/file delete/folder/nopermspa/dir/file2 delete/folder/nopermspa/dir/file3 delete/folder/nopermspa/dir/file4
		chmod 777 delete/folder/nopermspa/dir/file delete/folder/nopermspa/dir/file2 delete/folder/nopermspa/dir/file3 delete/folder/nopermspa/dir/file4
		chmod 777 delete/folder/nopermspa/dir
		chmod 000 delete/folder/nopermspa

	# Test 4 : No Perms folder : Full perms parent, No perms folder, Fill
		mkdir delete/folder/noperms
		createRandomFile delete/folder/noperms/file delete/folder/noperms/file2 delete/folder/noperms/file3 delete/folder/noperms/file4
		chmod 777 delete/folder/noperms/file delete/folder/noperms/file2 delete/folder/noperms/file3 delete/folder/noperms/file4
		chmod 000 delete/folder/noperms

	# Test 5 : Folder in Folder (No perms) : Full perms parent, Full perms folder, Fill with dir
		mkdir delete/folder/dire
		chmod 777 delete/folder/dire
		createRandomFile delete/folder/dire/file delete/folder/dire/file2 delete/folder/dire/file3 delete/folder/dire/file4
		chmod 777 delete/folder/dire/file delete/folder/dire/file2 delete/folder/dire/file3 delete/folder/dire/file4
		mkdir delete/folder/dire/inside
		createRandomFile delete/folder/dire/inside/file delete/folder/dire/inside/file2 delete/folder/dire/inside/file3 delete/folder/dire/inside/file4
		chmod 777 delete/folder/dire/inside/file delete/folder/dire/inside/file2 delete/folder/dire/inside/file3 delete/folder/dire/inside/file4
		chmod 000 delete/folder/dire/inside

	# Test 6 : Folder in Folder (No perms but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir delete/folder/dire2
		chmod 777 delete/folder/dire2
		createRandomFile delete/folder/dire2/file delete/folder/dire2/file2 delete/folder/dire2/file3 delete/folder/dire2/file4
		chmod 777 delete/folder/dire2/file delete/folder/dire2/file2 delete/folder/dire2/file3 delete/folder/dire2/file4
		mkdir delete/folder/dire2/inside
		chmod 000 delete/folder/dire2/inside

	# Test 7 : Folder in Folder (Read Only but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir delete/folder/dire3
		chmod 777 delete/folder/dire3
		createRandomFile delete/folder/dire3/file delete/folder/dire3/file2 delete/folder/dire3/file3 delete/folder/dire3/file4
		chmod 777 delete/folder/dire3/file delete/folder/dire3/file2 delete/folder/dire3/file3 delete/folder/dire3/file4
		mkdir delete/folder/dire3/inside
		chmod 444 delete/folder/dire3/inside

	# Test 8 : Folder in folder in folder ... : Full Perms EveryWhere, Fill with dir, file
		mkdir delete/folder/dire4
		chmod 777 delete/folder/dire4
		createRandomFile delete/folder/dire4/file delete/folder/dire4/file2 delete/folder/dire4/file3 delete/folder/dire4/file4
		chmod 777 delete/folder/dire4/file delete/folder/dire4/file2 delete/folder/dire4/file3 delete/folder/dire4/file4
		mkdir delete/folder/dire4/inside
		chmod 777 delete/folder/dire4/inside
		createRandomFile delete/folder/dire4/inside/file delete/folder/dire4/inside/file2 delete/folder/dire4/inside/file3 delete/folder/dire4/inside/file4
		chmod 777 delete/folder/dire4/inside/file delete/folder/dire4/inside/file2 delete/folder/dire4/inside/file3 delete/folder/dire4/inside/file4
		mkdir delete/folder/dire4/inside/inside
		chmod 777 delete/folder/dire4/inside/inside
		createRandomFile delete/folder/dire4/inside/inside/file delete/folder/dire4/inside/inside/file2 delete/folder/dire4/inside/inside/file3 delete/folder/dire4/inside/inside/file4
		chmod 777 delete/folder/dire4/inside/inside/file delete/folder/dire4/inside/inside/file2 delete/folder/dire4/inside/inside/file3 delete/folder/dire4/inside/inside/file4
		mkdir delete/folder/dire4/inside/inside/inside
		chmod 777 delete/folder/dire4/inside/inside/inside
		createRandomFile delete/folder/dire4/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/file4
		chmod 777 delete/folder/dire4/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/file4
		mkdir delete/folder/dire4/inside/inside/inside/inside
		chmod 777 delete/folder/dire4/inside/inside/inside/inside
		createRandomFile delete/folder/dire4/inside/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/inside/file4
		chmod 777 delete/folder/dire4/inside/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/inside/file4
