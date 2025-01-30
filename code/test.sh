# Create Working Directory
mkdir unitTest/temp
chmod 777 unitTest/temp/

##---------------------------------------------------------------------------------------------------##
	# Create Folder Full perms with 3 file.
	mkdir unitTest/temp/full
	chmod 777 unitTest/temp/full/

	# Test 1 : Classic : Full perms files, Full Perms Folder.
		touch unitTest/temp/full/classic
		chmod 777 unitTest/temp/full/classic
	# Test 2 : Classic with "../" in URL : Full perms files, Full perms folder.
		touch unitTest/temp/full/noback
		chmod 777 unitTest/temp/full/noback
	# Test 3 : Classic : No perms file, Full perms folder |
		touch unitTest/temp/full/noperms
		chmod 000 unitTest/temp/full/noperms

	# Create Folder with no perms with 1 file
	mkdir unitTest/temp/cant


	# Test 4 : No Perms : Full perms Files, No Perms Folder
	#			Response for 'unitTest/temp/cant/tryme' : 403 "Forbidden" | File NOT Delete
		touch unitTest/temp/cant/tryme
		chmod 777 unitTest/temp/cant/tryme
		chmod 000 unitTest/temp/cant/

	# Create Folder with only perms to read with 1 file
	mkdir unitTest/temp/readme
	# Test 5 : Full Permes FIles, Only read perms. |
	#			Response for '/unitTest/temp/readme/deleteme' : 403 "Forbidden" | File NOT Delete
		touch unitTest/temp/readme/deleteme
		chmod 777 unitTest/temp/readme/deleteme
		chmod 444 unitTest/temp/readme/

##---------------------------------------------------------------------------------------------------##
	# Create Working Directory
	mkdir unitTest/temp/folder
	chmod 777 unitTest/temp/folder/

	# Test 1 : Empty : Full perms parent, Full perms folder, Empty
		mkdir unitTest/temp/folder/empty
		chmod 777 unitTest/temp/folder/empty
	# Test 2 : Classic : Full perms parent, Full perms folder, Fill
		mkdir unitTest/temp/folder/classic
		chmod 777 unitTest/temp/folder/classic
		touch unitTest/temp/folder/classic/file unitTest/temp/folder/classic/file2 unitTest/temp/folder/classic/file3 unitTest/temp/folder/classic/file4
		chmod 777 unitTest/temp/folder/classic/file unitTest/temp/folder/classic/file2 unitTest/temp/folder/classic/file3 unitTest/temp/folder/classic/file4
	# Test 3 : No Perms parent : No perms parent, Full perms folder, Fill
		mkdir unitTest/temp/folder/nopermspa
		mkdir unitTest/temp/folder/nopermspa/dir
		touch unitTest/temp/folder/nopermspa/dir/file unitTest/temp/folder/nopermspa/dir/file2 unitTest/temp/folder/nopermspa/dir/file3 unitTest/temp/folder/nopermspa/dir/file4
		chmod 777 unitTest/temp/folder/nopermspa/dir/file unitTest/temp/folder/nopermspa/dir/file2 unitTest/temp/folder/nopermspa/dir/file3 unitTest/temp/folder/nopermspa/dir/file4
		chmod 777 unitTest/temp/folder/nopermspa/dir
		chmod 000 unitTest/temp/folder/nopermspa

	# Test 4 : No Perms folder : Full perms parent, No perms folder, Fill
		mkdir unitTest/temp/folder/noperms
		touch unitTest/temp/folder/noperms/file unitTest/temp/folder/noperms/file2 unitTest/temp/folder/noperms/file3 unitTest/temp/folder/noperms/file4
		chmod 777 unitTest/temp/folder/noperms/file unitTest/temp/folder/noperms/file2 unitTest/temp/folder/noperms/file3 unitTest/temp/folder/noperms/file4
		chmod 000 unitTest/temp/folder/noperms

	# Test 5 : Folder in Folder (No perms) : Full perms parent, Full perms folder, Fill with dir
		mkdir unitTest/temp/folder/dire
		chmod 777 unitTest/temp/folder/dire
		touch unitTest/temp/folder/dire/file unitTest/temp/folder/dire/file2 unitTest/temp/folder/dire/file3 unitTest/temp/folder/dire/file4
		chmod 777 unitTest/temp/folder/dire/file unitTest/temp/folder/dire/file2 unitTest/temp/folder/dire/file3 unitTest/temp/folder/dire/file4
		mkdir unitTest/temp/folder/dire/inside
		touch unitTest/temp/folder/dire/inside/file unitTest/temp/folder/dire/inside/file2 unitTest/temp/folder/dire/inside/file3 unitTest/temp/folder/dire/inside/file4
		chmod 777 unitTest/temp/folder/dire/inside/file unitTest/temp/folder/dire/inside/file2 unitTest/temp/folder/dire/inside/file3 unitTest/temp/folder/dire/inside/file4
		chmod 000 unitTest/temp/folder/dire/inside

	# Test 6 : Folder in Folder (No perms but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir unitTest/temp/folder/dire2
		chmod 777 unitTest/temp/folder/dire2
		touch unitTest/temp/folder/dire2/file unitTest/temp/folder/dire2/file2 unitTest/temp/folder/dire2/file3 unitTest/temp/folder/dire2/file4
		chmod 777 unitTest/temp/folder/dire2/file unitTest/temp/folder/dire2/file2 unitTest/temp/folder/dire2/file3 unitTest/temp/folder/dire2/file4
		mkdir unitTest/temp/folder/dire2/inside
		chmod 000 unitTest/temp/folder/dire2/inside

	# Test 7 : Folder in Folder (Read Only but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir unitTest/temp/folder/dire3
		chmod 777 unitTest/temp/folder/dire3
		touch unitTest/temp/folder/dire3/file unitTest/temp/folder/dire3/file2 unitTest/temp/folder/dire3/file3 unitTest/temp/folder/dire3/file4
		chmod 777 unitTest/temp/folder/dire3/file unitTest/temp/folder/dire3/file2 unitTest/temp/folder/dire3/file3 unitTest/temp/folder/dire3/file4
		mkdir unitTest/temp/folder/dire3/inside
		chmod 444 unitTest/temp/folder/dire3/inside

	# Test 8 : Folder in folder in folder ... : Full Perms EveryWhere, Fill with dir, file
		mkdir unitTest/temp/folder/dire4
		chmod 777 unitTest/temp/folder/dire4
		touch unitTest/temp/folder/dire4/file unitTest/temp/folder/dire4/file2 unitTest/temp/folder/dire4/file3 unitTest/temp/folder/dire4/file4
		chmod 777 unitTest/temp/folder/dire4/file unitTest/temp/folder/dire4/file2 unitTest/temp/folder/dire4/file3 unitTest/temp/folder/dire4/file4
		mkdir unitTest/temp/folder/dire4/inside
		chmod 777 unitTest/temp/folder/dire4/inside
		touch unitTest/temp/folder/dire4/inside/file unitTest/temp/folder/dire4/inside/file2 unitTest/temp/folder/dire4/inside/file3 unitTest/temp/folder/dire4/inside/file4
		chmod 777 unitTest/temp/folder/dire4/inside/file unitTest/temp/folder/dire4/inside/file2 unitTest/temp/folder/dire4/inside/file3 unitTest/temp/folder/dire4/inside/file4
		mkdir unitTest/temp/folder/dire4/inside/inside
		chmod 777 unitTest/temp/folder/dire4/inside/inside
		touch unitTest/temp/folder/dire4/inside/inside/file unitTest/temp/folder/dire4/inside/inside/file2 unitTest/temp/folder/dire4/inside/inside/file3 unitTest/temp/folder/dire4/inside/inside/file4
		chmod 777 unitTest/temp/folder/dire4/inside/inside/file unitTest/temp/folder/dire4/inside/inside/file2 unitTest/temp/folder/dire4/inside/inside/file3 unitTest/temp/folder/dire4/inside/inside/file4
		mkdir unitTest/temp/folder/dire4/inside/inside/inside
		chmod 777 unitTest/temp/folder/dire4/inside/inside/inside
		touch unitTest/temp/folder/dire4/inside/inside/inside/file unitTest/temp/folder/dire4/inside/inside/inside/file2 unitTest/temp/folder/dire4/inside/inside/inside/file3 unitTest/temp/folder/dire4/inside/inside/inside/file4
		chmod 777 unitTest/temp/folder/dire4/inside/inside/inside/file unitTest/temp/folder/dire4/inside/inside/inside/file2 unitTest/temp/folder/dire4/inside/inside/inside/file3 unitTest/temp/folder/dire4/inside/inside/inside/file4
		mkdir unitTest/temp/folder/dire4/inside/inside/inside/inside
		chmod 777 unitTest/temp/folder/dire4/inside/inside/inside/inside
		touch unitTest/temp/folder/dire4/inside/inside/inside/inside/file unitTest/temp/folder/dire4/inside/inside/inside/inside/file2 unitTest/temp/folder/dire4/inside/inside/inside/inside/file3 unitTest/temp/folder/dire4/inside/inside/inside/inside/file4
		chmod 777 unitTest/temp/folder/dire4/inside/inside/inside/inside/file unitTest/temp/folder/dire4/inside/inside/inside/inside/file2 unitTest/temp/folder/dire4/inside/inside/inside/inside/file3 unitTest/temp/folder/dire4/inside/inside/inside/inside/file4