#!/bin/bash

# Ubuntu or MacOS (Warning!!! For Windows use build.ps (powershell script))
# shell script for building and possible (re)installing

echo
echo "##################################################"
echo "Welcome to Simple Graphics Library helper script #"
echo "##################################################"
echo

############################################################
# Help                                                     #
############################################################
Help()
{
   	# Display Help
   	echo
	echo "#####################################"
   	echo "given options:                      #"
	echo "-d : Debug build                    #"
   	echo "-r : Release build                  #"
   	echo "-c : Clear build                    #"
	echo "-i : Install to the /usr/local/lib  #"
	echo "-e : Build and run examples data    #"
	echo "-z : Remove library from installed  #"
	echo "#####################################"
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
REMOVE_LIBRARY=false # remove library from folder where installed
SYSTEM_TYPE="$(uname -s)"
PATH_INC=
PATH_LIB=

echo
echo "Your system is $SYSTEM_TYPE"
echo

case $SYSTEM_TYPE in 
	Linux)
		PATH_INC=/usr/include
		PATH_LIB=/usr/lib
		;;
	Darwin)
		PATH_INC=/usr/local/include
		PATH_LIB=/usr/local/lib
		;;
esac

# Get the options
while getopts ":rcidez" flag
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
		z)	# Remove library
			REMOVE_LIBRARY=true
			;;
	   \?)	# Invalid option
	   		echo
			echo "Error: Invalid option - ${OPTARG}"
	   		echo
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

# Remove library from /usr/**
if [ $REMOVE_LIBRARY == true ]
then
	rm -rf $PATH_INC/SGR
	rm -f $PATH_LIB/libSGR.*
	echo
	echo "Library was removed succesfull"
	echo
fi

# Clean without build
if [ $BUILD == false ]
then
	if [ $CLEAN ]
	then
		rm -rf build
		rm -rf examplesData/build
		echo
		echo "Build folder was cleared"
		echo
	fi
	if [ $INSTALL == true ]
	then
		echo
		echo "Cannot install without building"
		echo
	fi
	exit
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
			rm -rf $PATH_INC/SGR
			mkdir $PATH_INC/SGR
			cp -rf $entry/include/* $PATH_INC/SGR
			cp -f $entry/lib/shared/* $PATH_LIB
			echo
			echo "Installed in $PATH_INC/SGR and $PATH_LIB/SGR"
			echo
		fi
		tar -cf $entry.tar $entry
	done
	cd ..
else
	if [ $INSTALL == true ]
	then
		echo
		echo "Cannot install not release build type"
		echo
	fi
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