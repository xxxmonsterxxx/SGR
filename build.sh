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

type=debug #build type debug or release
install=false #install library or not

# Get the options
while getopts ":hrcif" flag
do
    case $flag in
		h)	#Help message request
			Help
			exit;;
        r) 	#Build type is release
			type=release;;
		c) 	clean=true;;
		i)	install=true;;
		f)	if [ $clean ]
			then
				rm -rf build
			fi;;
	   \?)	# Invalid option
			echo "Error: Invalid option - ${OPTARG}"
			exit;;
    esac
done

mkdir build
cd build

case $type in
	debug)		cmake .. -DRELEASE=FALSE ;;
	release)	cmake .. -DRELEASE=TRUE ;;
esac

if [ $clean ]
then
	cmake --build . --clean-first -- -j
else
	cmake --build . -- -j
fi

if [ $type == release ]
then
	cd release
	rm -rf *.tar
	for entry in `ls $search_dir`; do
		if [ $install ]
		then
			mkdir /usr/local/include/SGR
			cp -rf $entry/include/*.h /usr/local/include/SGR
			cp -f $entry/lib/shared/* /usr/local/lib
			tar -cf $entry.tar $entry
		fi
	done
fi

#############