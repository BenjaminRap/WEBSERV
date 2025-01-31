# Create Working Directory
mkdir -p delete
chmod 777 delete/

##--------------------------------------------------------------------------------------------------##
	#Tricky tests
	# Test 1: symlink loop with parent no right
		mkdir -p delete/trickyNoRight/symlinkParent
		ln -s delete/trickyNoRight/symlinkParent delete/trickyNoRight/symlinkParent/symlinkLoop
	# Test 2 : symlink loop with parent right
		mkdir -p delete/trickyRight/symlinkParent
		chmod 777 delete/trickyRight
		ln -s delete/trickyRight/symlinkParent delete/trickyRight/symlinkParent/symlinkLoop
	# Test 3: symlink pointing outside root with parent right
		mkdir -p ../outsideRoot
		chmod 777 ../outsideRoot
		touch ../outsideRoot/outsideRoot.txt
		(cd delete/trickyRight/ && ln -s ../../../outsideRoot outsideLink)
	# Test 4: symlink pointing outside root with parent no right
		(cd delete/trickyNoRight/ && ln -s ../../../outsideRoot outsideLink)
	# Test 5: immutable file
		touch delete/immutable.txt
		chattr +i delete/immutable.txt

##---------------------------------------------------------------------------------------------------##
	# Create Folder Full perms with 3 file.
	mkdir -p delete/full
	chmod 777 delete/full/

	# Test 1 : Classic : Full perms files, Full Perms Folder.
		touch delete/full/classic
		chmod 777 delete/full/classic
	# Test 2 : Classic with "../" in URL : Full perms files, Full perms folder.
		touch delete/full/noback
		chmod 777 delete/full/noback
	# Test 3 : Classic : No perms file, Full perms folder |
		touch delete/full/noperms
		chmod 000 delete/full/noperms

	# Create Folder with no perms with 1 file
	mkdir -p delete/cant


	# Test 4 : No Perms : Full perms Files, No Perms Folder
	#			Response for 'delete/cant/tryme' : 403 "Forbidden" | File NOT Delete
		touch delete/cant/tryme
		chmod 777 delete/cant/tryme
		chmod 000 delete/cant/

	# Create Folder with only perms to read with 1 file
	mkdir -p delete/readme
	# Test 5 : Full Permes FIles, Only read perms. |
	#			Response for '/delete/readme/deleteme' : 403 "Forbidden" | File NOT Delete
		touch delete/readme/deleteme
		chmod 777 delete/readme/deleteme
		chmod 444 delete/readme/

##---------------------------------------------------------------------------------------------------##
	# Create Working Directory
	mkdir -p delete/folder
	chmod 777 delete/folder/

	# Test 1 : Empty : Full perms parent, Full perms folder, Empty
		mkdir -p delete/folder/empty
		chmod 777 delete/folder/empty
	# Test 2 : Classic : Full perms parent, Full perms folder, Fill
		mkdir -p delete/folder/classic
		chmod 777 delete/folder/classic
		touch delete/folder/classic/file delete/folder/classic/file2 delete/folder/classic/file3 delete/folder/classic/file4
		chmod 777 delete/folder/classic/file delete/folder/classic/file2 delete/folder/classic/file3 delete/folder/classic/file4
	# Test 3 : No Perms parent : No perms parent, Full perms folder, Fill
		mkdir -p delete/folder/nopermspa
		mkdir -p delete/folder/nopermspa/dir
		touch delete/folder/nopermspa/dir/file delete/folder/nopermspa/dir/file2 delete/folder/nopermspa/dir/file3 delete/folder/nopermspa/dir/file4
		chmod 777 delete/folder/nopermspa/dir/file delete/folder/nopermspa/dir/file2 delete/folder/nopermspa/dir/file3 delete/folder/nopermspa/dir/file4
		chmod 777 delete/folder/nopermspa/dir
		chmod 000 delete/folder/nopermspa

	# Test 4 : No Perms folder : Full perms parent, No perms folder, Fill
		mkdir -p delete/folder/noperms
		touch delete/folder/noperms/file delete/folder/noperms/file2 delete/folder/noperms/file3 delete/folder/noperms/file4
		chmod 777 delete/folder/noperms/file delete/folder/noperms/file2 delete/folder/noperms/file3 delete/folder/noperms/file4
		chmod 000 delete/folder/noperms

	# Test 5 : Folder in Folder (No perms) : Full perms parent, Full perms folder, Fill with dir
		mkdir -p delete/folder/dire
		chmod 777 delete/folder/dire
		touch delete/folder/dire/file delete/folder/dire/file2 delete/folder/dire/file3 delete/folder/dire/file4
		chmod 777 delete/folder/dire/file delete/folder/dire/file2 delete/folder/dire/file3 delete/folder/dire/file4
		mkdir -p delete/folder/dire/inside
		touch delete/folder/dire/inside/file delete/folder/dire/inside/file2 delete/folder/dire/inside/file3 delete/folder/dire/inside/file4
		chmod 777 delete/folder/dire/inside/file delete/folder/dire/inside/file2 delete/folder/dire/inside/file3 delete/folder/dire/inside/file4
		chmod 000 delete/folder/dire/inside

	# Test 6 : Folder in Folder (No perms but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir -p delete/folder/dire2
		chmod 777 delete/folder/dire2
		touch delete/folder/dire2/file delete/folder/dire2/file2 delete/folder/dire2/file3 delete/folder/dire2/file4
		chmod 777 delete/folder/dire2/file delete/folder/dire2/file2 delete/folder/dire2/file3 delete/folder/dire2/file4
		mkdir -p delete/folder/dire2/inside
		chmod 000 delete/folder/dire2/inside

	# Test 7 : Folder in Folder (Read Only but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir -p delete/folder/dire3
		chmod 777 delete/folder/dire3
		touch delete/folder/dire3/file delete/folder/dire3/file2 delete/folder/dire3/file3 delete/folder/dire3/file4
		chmod 777 delete/folder/dire3/file delete/folder/dire3/file2 delete/folder/dire3/file3 delete/folder/dire3/file4
		mkdir -p delete/folder/dire3/inside
		chmod 444 delete/folder/dire3/inside

	# Test 8 : Folder in folder in folder ... : Full Perms EveryWhere, Fill with dir, file
		mkdir -p delete/folder/dire4
		chmod 777 delete/folder/dire4
		touch delete/folder/dire4/file delete/folder/dire4/file2 delete/folder/dire4/file3 delete/folder/dire4/file4
		chmod 777 delete/folder/dire4/file delete/folder/dire4/file2 delete/folder/dire4/file3 delete/folder/dire4/file4
		mkdir -p delete/folder/dire4/inside
		chmod 777 delete/folder/dire4/inside
		touch delete/folder/dire4/inside/file delete/folder/dire4/inside/file2 delete/folder/dire4/inside/file3 delete/folder/dire4/inside/file4
		chmod 777 delete/folder/dire4/inside/file delete/folder/dire4/inside/file2 delete/folder/dire4/inside/file3 delete/folder/dire4/inside/file4
		mkdir -p delete/folder/dire4/inside/inside
		chmod 777 delete/folder/dire4/inside/inside
		touch delete/folder/dire4/inside/inside/file delete/folder/dire4/inside/inside/file2 delete/folder/dire4/inside/inside/file3 delete/folder/dire4/inside/inside/file4
		chmod 777 delete/folder/dire4/inside/inside/file delete/folder/dire4/inside/inside/file2 delete/folder/dire4/inside/inside/file3 delete/folder/dire4/inside/inside/file4
		mkdir -p delete/folder/dire4/inside/inside/inside
		chmod 777 delete/folder/dire4/inside/inside/inside
		touch delete/folder/dire4/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/file4
		chmod 777 delete/folder/dire4/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/file4
		mkdir -p delete/folder/dire4/inside/inside/inside/inside
		chmod 777 delete/folder/dire4/inside/inside/inside/inside
		touch delete/folder/dire4/inside/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/inside/file4
		chmod 777 delete/folder/dire4/inside/inside/inside/inside/file delete/folder/dire4/inside/inside/inside/inside/file2 delete/folder/dire4/inside/inside/inside/inside/file3 delete/folder/dire4/inside/inside/inside/inside/file4