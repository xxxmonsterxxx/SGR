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


SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )/$(basename "$SOURCE")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"

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
USER_CONFIG_FILE=

echo
echo "Your system is $SYSTEM_TYPE"
echo

case $SYSTEM_TYPE in 
	Linux)
		PATH_INC=/usr/include
		PATH_LIB=/usr/lib
		USER_CONFIG_FILE="$HOME/.bashrc"
		;;
	Darwin)
		PATH_INC=/usr/local/include
		PATH_LIB=/usr/local/lib
		USER_CONFIG_FILE="$HOME/.zshrc"
		;;
esac

# Get the options
while getopts ":rcidezh" flag
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
		h)  # Help
			Help
			exit
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
	debug)		cmake .. -DCMAKE_BUILD_TYPE=Debug --install-prefix $SCRIPT_DIR/build/ ;;
	release)	cmake .. -DCMAKE_BUILD_TYPE=Release --install-prefix /usr/local/;;
esac

# Need to clean?
if [ $CLEAN ]
then
	cmake --build . --CLEAN-first -- -j
else
	cmake --build . -- -j
	if [ $EXAMPLE_BUILD == true ]
	then
		cmake .. -DBUILD_EXAMPLE=TRUE
		cmake --build . -- -j
	fi
fi

# If build type is release (with/out install option)
if [ $BUILD_TYPE == release ]
then
	cd release
	rm -rf *.tar
	for entry in `ls $search_dir`; do
		if [ $INSTALL == true ]
		then
			#setting environement variables
			if [ -f  ]; then
				if ! grep -q 'export CMAKE_PREFIX_PATH=/usr/local/include/SGR:CMAKE_PREFIX_PATH' $USER_CONFIG_FILE; then
					echo >> $USER_CONFIG_FILE
					echo 'export CMAKE_PREFIX_PATH=/usr/local/include/SGR:CMAKE_PREFIX_PATH #for cmake package_find command' >> $USER_CONFIG_FILE
				fi
				
				if ! grep -q 'export SGR_LIB=/usr/local/lib/libSGR.dylib' $USER_CONFIG_FILE; then
					echo 'export SGR_LIB=/usr/local/lib/libSGR.dylib' >> $USER_CONFIG_FILE
				fi
				echo "SDK environement added to user config file successfully."
			fi
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
	./build/exampleApplication/SGR_test
fi

############################################################