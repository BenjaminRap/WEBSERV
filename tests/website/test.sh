#				List of tests for the website.
# File Test :
#	Full perms files, Full Perms folder. : '/temp/full/classic' | 204 "No Content"
#	Full perms files, Full perms folder. (With '../' in URL) : '/temp/full/../../temp/full/noback' | 204 "No Content"
#	No perms file, Full perms folder : '' | 204 "No Content"
#	Full perms files, No Perms folder : '/temp/cant/tryme' | 403 "Forbidden"
#	Full perms files, Only read perms folder : '/temp/readme/deleteme' | 403 "Forbidden"
#
# Folder Test :
#	Empty : Full perms parent, Full perms folder, Empty : '/temp/folder/empty' | 204 "No Content"
#	Classic : Full perms parent, Full perms folder, Fill : '/temp/folder/classic' | 204 "No Content"
#	No Perms parent : No perms parent, Full perms folder, Fill : '/temp/folder/nopermspa' | 500 "Internal Server Error"
#	No Perms folder : Full perms parent, No perms folder, Fill : '/temp/folder/noperms' | 500 "Internal Server Error"
#	Folder in Folder (No perms) : Full perms parent, Full perms folder, Fill with dir : '/temp/folder/dire/' | 500 "Internal Server Error"
#	Folder in Folder (No perms but empty) : Full perms parent, Full perms folder, Fill with dir : '/temp/folder/dire2/' | 204 "No Content"
#	Folder in Folder (Read Only but empty) : Full perms parent, Full perms folder, Fill with dir : '/temp/folder/dire3/' | 204 "No Content"
#	Folder in folder in folder ... : Full Perms EveryWhere, Fill with dir, file : '/temp/folder/dire4/' | 204 "No Content"

# Create Working Directory
mkdir temp
chmod 777 temp/

##----------------------------------	FILE	TEST	------------------------------------------##

	# Create Folder Full perms with 3 file.
	mkdir temp/full
	chmod 777 temp/full/

	# Test 1 : Classic : Full perms files, Full Perms Folder.
		touch temp/full/classic
		chmod 777 temp/full/classic
	# Test 2 : Classic with "../" in URL : Full perms files, Full perms folder.
		touch temp/full/noback
		chmod 777 temp/full/noback
	# Test 3 : Classic : No perms file, Full perms folder |
		touch temp/full/noperms
		chmod 000 temp/full/noperms

	# Create Folder with no perms with 1 file
	mkdir temp/cant
	# Test 4 : No Perms : Full perms Files, No Perms Folder
	#			Response for 'temp/cant/tryme' : 403 "Forbidden" | File NOT Delete
		touch temp/cant/tryme
		chmod 777 temp/cant/tryme

	chmod 000 temp/cant/


	# Create Folder with only perms to read with 1 file
	mkdir temp/readme
	# Test 5 : Full Permes FIles, Only read perms. |
	#			Response for '/temp/readme/deleteme' : 403 "Forbidden" | File NOT Delete
		touch temp/readme/deleteme
		chmod 777 temp/readme/deleteme
	chmod 444 temp/readme/

##---------------------------------------------------------------------------------------------------##
##----------------------------------	FOLDER	TEST	------------------------------------------##

	# Create Working Directory
		mkdir temp/folder
		chmod 777 temp/folder/

	# Test 1 : Empty : Full perms parent, Full perms folder, Empty
		mkdir temp/folder/empty
		chmod 777 temp/folder/empty
	# Test 2 : Classic : Full perms parent, Full perms folder, Fill
		mkdir temp/folder/classic
		chmod 777 temp/folder/classic
		touch temp/folder/classic/file temp/folder/classic/file2 temp/folder/classic/file3 temp/folder/classic/file4
		chmod 777 temp/folder/classic/file temp/folder/classic/file2 temp/folder/classic/file3 temp/folder/classic/file4
	# Test 3 : No Perms parent : No perms parent, Full perms folder, Fill
		mkdir temp/folder/nopermspa
		mkdir temp/folder/nopermspa/dir
		touch temp/folder/nopermspa/dir/file temp/folder/nopermspa/dir/file2 temp/folder/nopermspa/dir/file3 temp/folder/nopermspa/dir/file4
		chmod 777 temp/folder/nopermspa/dir/file temp/folder/nopermspa/dir/file2 temp/folder/nopermspa/dir/file3 temp/folder/nopermspa/dir/file4
		chmod 777 temp/folder/nopermspa/dir
		chmod 000 temp/folder/nopermspa

	# Test 4 : No Perms folder : Full perms parent, No perms folder, Fill
		mkdir temp/folder/noperms
		touch temp/folder/noperms/file temp/folder/noperms/file2 temp/folder/noperms/file3 temp/folder/noperms/file4
		chmod 777 temp/folder/noperms/file temp/folder/noperms/file2 temp/folder/noperms/file3 temp/folder/noperms/file4
		chmod 000 temp/folder/noperms

	# Test 5 : Folder in Folder (No perms) : Full perms parent, Full perms folder, Fill with dir
		mkdir temp/folder/dire
		chmod 777 temp/folder/dire
		touch temp/folder/dire/file temp/folder/dire/file2 temp/folder/dire/file3 temp/folder/dire/file4
		chmod 777 temp/folder/dire/file temp/folder/dire/file2 temp/folder/dire/file3 temp/folder/dire/file4
		mkdir temp/folder/dire/inside
		touch temp/folder/dire/inside/file temp/folder/dire/inside/file2 temp/folder/dire/inside/file3 temp/folder/dire/inside/file4
		chmod 777 temp/folder/dire/inside/file temp/folder/dire/inside/file2 temp/folder/dire/inside/file3 temp/folder/dire/inside/file4
		chmod 000 temp/folder/dire/inside

	# Test 6 : Folder in Folder (No perms but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir temp/folder/dire2
		chmod 777 temp/folder/dire2
		touch temp/folder/dire2/file temp/folder/dire2/file2 temp/folder/dire2/file3 temp/folder/dire2/file4
		chmod 777 temp/folder/dire2/file temp/folder/dire2/file2 temp/folder/dire2/file3 temp/folder/dire2/file4
		mkdir temp/folder/dire2/inside
		chmod 000 temp/folder/dire2/inside

	# Test 7 : Folder in Folder (Read Only but empty) : Full perms parent, Full perms folder, Fill with dir
		mkdir temp/folder/dire3
		chmod 777 temp/folder/dire3
		touch temp/folder/dire3/file temp/folder/dire3/file2 temp/folder/dire3/file3 temp/folder/dire3/file4
		chmod 777 temp/folder/dire3/file temp/folder/dire3/file2 temp/folder/dire3/file3 temp/folder/dire3/file4
		mkdir temp/folder/dire3/inside
		chmod 444 temp/folder/dire3/inside

	# Test 8 : Folder in folder in folder ... : Full Perms EveryWhere, Fill with dir, file
		mkdir temp/folder/dire4
		chmod 777 temp/folder/dire4
		touch temp/folder/dire4/file temp/folder/dire4/file2 temp/folder/dire4/file3 temp/folder/dire4/file4
		chmod 777 temp/folder/dire4/file temp/folder/dire4/file2 temp/folder/dire4/file3 temp/folder/dire4/file4
		mkdir temp/folder/dire4/inside
		chmod 777 temp/folder/dire4/inside
		touch temp/folder/dire4/inside/file temp/folder/dire4/inside/file2 temp/folder/dire4/inside/file3 temp/folder/dire4/inside/file4
		chmod 777 temp/folder/dire4/inside/file temp/folder/dire4/inside/file2 temp/folder/dire4/inside/file3 temp/folder/dire4/inside/file4
		mkdir temp/folder/dire4/inside/inside
		chmod 777 temp/folder/dire4/inside/inside
		touch temp/folder/dire4/inside/inside/file temp/folder/dire4/inside/inside/file2 temp/folder/dire4/inside/inside/file3 temp/folder/dire4/inside/inside/file4
		chmod 777 temp/folder/dire4/inside/inside/file temp/folder/dire4/inside/inside/file2 temp/folder/dire4/inside/inside/file3 temp/folder/dire4/inside/inside/file4
		mkdir temp/folder/dire4/inside/inside/inside
		chmod 777 temp/folder/dire4/inside/inside/inside
		touch temp/folder/dire4/inside/inside/inside/file temp/folder/dire4/inside/inside/inside/file2 temp/folder/dire4/inside/inside/inside/file3 temp/folder/dire4/inside/inside/inside/file4
		chmod 777 temp/folder/dire4/inside/inside/inside/file temp/folder/dire4/inside/inside/inside/file2 temp/folder/dire4/inside/inside/inside/file3 temp/folder/dire4/inside/inside/inside/file4
		mkdir temp/folder/dire4/inside/inside/inside/inside
		chmod 777 temp/folder/dire4/inside/inside/inside/inside
		touch temp/folder/dire4/inside/inside/inside/inside/file temp/folder/dire4/inside/inside/inside/inside/file2 temp/folder/dire4/inside/inside/inside/inside/file3 temp/folder/dire4/inside/inside/inside/inside/file4
		chmod 777 temp/folder/dire4/inside/inside/inside/inside/file temp/folder/dire4/inside/inside/inside/inside/file2 temp/folder/dire4/inside/inside/inside/inside/file3 temp/folder/dire4/inside/inside/inside/inside/file4