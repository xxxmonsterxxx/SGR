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
		exit;
	fi
	echo
	echo "Cannot install without building"
	echo
	exit;
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
			echo
			echo "Installed in /usr/local/include  /usr/local/lib"
			echo
		fi
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