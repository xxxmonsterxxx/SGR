#!/bin/bash

# Ubuntu or MacOS (Warning!!! For Windows use build.ps (powershell script))
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
	echo "-i : Install to the /usr/local/lib"
	echo "-e : Build and run examples data"
   	echo
}

############################################################
############################################################

BUILD=false # do we need build or not?
INSTALL=false # install library or not
BUILD_TYPE=debug # release or debug
CLEAN= # clean or not
EXAMPLE_BUILD=false # need to build example project?
OPTS_PRESENTED=false # options provided

# Get the options
while getopts ":rcide" flag
do
	OPTS_PRESENTED=true
    case $flag in
		d) 	# Build type is debug
			BUILD=true
			BUILD_TYPE=debug
			;;
        r) 	# Build type is release
			BUILD=true
			BUILD_TYPE=release
			;;
		c) 	# Need to CLEAN
			CLEAN=true
			;;
		i)	# Need to INSTALL
			INSTALL=true
			;;
		e)	# Example project
			BUILD=true
			EXAMPLE_BUILD=true
			;;
	   \?)	# Invalid option
			echo "Error: Invalid option - ${OPTARG}"
			Help
			exit
			;;
    esac
done

# If not option was provided
if [ $OPTS_PRESENTED == false ]
then
	Help
	exit
fi

# Clean without build
if [ $BUILD == false ]
then
	if [ $CLEAN ]
	then
		rm -rf build
		echo "Build folder was cleared"
		exit;
	fi
fi

mkdir build
cd build

# Choose build type
case $BUILD_TYPE in
	debug)		cmake .. -DRELEASE=FALSE ;;
	release)	cmake .. -DRELEASE=TRUE ;;
esac

# Need to clean?
if [ $CLEAN ]
then
	cmake --build . --CLEAN-first -- -j
else
	cmake --build . -- -j
fi

# If build type is release (with/out install option)
if [ $BUILD_TYPE == release ]
then
	cd release
	rm -rf *.tar
	for entry in `ls $search_dir`; do
		if [ $INSTALL == true ]
		then
			mkdir /usr/local/include/SGR
			cp -rf $entry/include/*.h /usr/local/include/SGR
			cp -f $entry/lib/shared/* /usr/local/lib
			tar -cf $entry.tar $entry
			echo "Installed in /usr/local/include  /usr/local/lib"
		fi
	done
	cd ..
fi

cd ..


# Example build and run if requested
if [ $EXAMPLE_BUILD == true ]
then
	cd examplesData
	mkdir build
	cd build
	cmake ..
	cmake --build . -- -j
	./exampleApplication/SGR_test
fi

############################################################