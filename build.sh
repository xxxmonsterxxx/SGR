#!/bin/bash

# shell script for building and possible (re)installing

############################################################
# Help                                                     #
############################################################
Help()
{
   # Display Help
   echo
   echo "Help for Simple Graphics Library build shell script."
   echo
   echo "options:"
   echo "-h Help."
   echo "-r Release build. By default build type is debug."
   echo "-c Clear build."
   echo
}

############################################################
############################################################

type=debug

# Get the options
while getopts ":hrc" flag
do
    case $flag in
		h)	#Help message request
			Help
			exit;;
        r) 	#Build type is release
			type=release;;
		c) 	clean=true;;
	   \?)	# Invalid option
			echo "Error: Invalid option - ${OPTARG}"
			exit;;
    esac
done

mkdir build
cd build

case "${type}" in
	debug) cmake .. -DRELEASE=FALSE ;;
	release) cmake .. -DRELEASE=TRUE -DVERSION=$ver ;;
esac

if [ $clean ]
then
	cmake --build . --clean-first -- -j
else
	cmake --build . -- -j
	cd release
	for entry in `ls $search_dir`; do
    	tar -cf ${entry}.tar ${entry}
	done
fi

#############