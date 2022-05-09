#!/bin/bash

# shell script for building and possible (re)installing

echo
echo "Welcome to Simple Graphics Library helper script"
echo

############################################################
# Help                                                     #
############################################################
Help()
{
   	# Display Help
   	echo
   	echo "given options:"
	echo "-d : Debug build"
   	echo "-r : Release build"
   	echo "-c : Clear build"
   	echo
}

############################################################
############################################################

need_build=false # do we need build or not?
install=false # install library or not

# Get the options
while getopts ":rcifd" flag
do
    case $flag in
		d) 	# Build type is debug
			need_build=true
			build_type=debug;;
        r) 	# Build type is release
			need_build=true
			build_type=release;;
		c) 	rm -rf build
			clean=true
			echo "Build folder was cleared"
			exit;;
		i)	install=true;;
	   \?)	# Invalid option
			echo "Error: Invalid option - ${OPTARG}"
			exit;;
    esac
done

if [ $need_build == false ]
then
	# Help message
	Help
	exit;
fi

mkdir build
cd build

case $build_type in
	debug)		cmake .. -DRELEASE=FALSE ;;
	release)	cmake .. -DRELEASE=TRUE ;;
esac

if [ $clean ]
then
	cmake --build . --clean-first -- -j
else
	cmake --build . -- -j
fi

if [ $build_type == release ]
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
			echo "Installed in /usr/local/include  /usr/local/lib"
		fi
	done
fi

############################################################