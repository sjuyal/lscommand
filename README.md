ls command in unix
--------------------
--------------------

Description
-------------------
Implementation of ls command in unix

First Execute: make

Then the follwing:

Options implemented:

Usage:  ./prog -[l|S|a|d|t|R] [file1 file2..] [dir1 dir2..]

­l : Long listing  format

	eg: ./prog -l filename[s] dirnames[s]

­R : Recursive
­
	eg: ./prog -R
	eg: ./prog -R dirname[s]

a : All
	
	eg: ./prog -a
	eg: ./prog -a filename[s] dirname[s]

­d : List  directory entries instead of contents

	eg: ./prog -d filename[s] dirname[s]

­S : Sort by file size

	eg: ./prog -S filename[s] dirname[s]
­
t : Sort by modification time

	eg: ./prog -t filename[s] dirname[s]

--Redirection can be done to the output file
