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
BUILD_TYPE=debug # release or debug
CLEAN= # clean or not
EXAMPLE_BUILD=false # need to build example project?
OPTS_PRESENTED=false # options provided
REMOVE_LIBRARY=false # remove library from folder where installed
SYSTEM_TYPE="$(uname -s)"
USER_CONFIG_FILE=
INSTALL_PATH=
INSTALL=false

echo
echo "Your system is $SYSTEM_TYPE"
echo

case $SYSTEM_TYPE in 
	Linux)
		INSTALL_PATH=/usr
		USER_CONFIG_FILE="$HOME/.bashrc"
		;;
	Darwin)
		INSTALL_PATH=/usr/local
		USER_CONFIG_FILE="$HOME/.zshrc"
		;;
esac

PATH_INC=$INSTALL_PATH/include
PATH_LIB=$INSTALL_PATH/lib

# Get the options
while getopts ":rcdezhi" flag
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
		i)	# Need to install
			INSTALL=true
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
	sudo rm -rf $PATH_INC/SGR
	sudo rm -f $PATH_LIB/libSGR.*
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
	exit
fi

mkdir build
cd build

# Choose build type
case $BUILD_TYPE in
	debug)		cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLE=FALSE --install-prefix $SCRIPT_DIR/build/ ;;
	release)	cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLE=FALSE --install-prefix $INSTALL_PATH ;;
esac

# Need to clean?
if [ $CLEAN ]
then
	cmake --build . --CLEAN-first -- -j
else
	cmake --build . -- -j

	if [ $BUILD_TYPE == release ] && [ $INSTALL == true ]
	then
		sudo mkdir $INSTALL_PATH/include/SGR
		sudo make install
	fi

	if [ $EXAMPLE_BUILD == true ]
	then
		cmake .. -DBUILD_EXAMPLE=TRUE
		cmake --build . -- -j
	fi
fi

# If build type is release (with/out install option)
if [ $BUILD_TYPE == release ] && [ $INSTALL ]
then
	#setting environement variables
	if [ -f  ]; then
		if ! grep -q "export CMAKE_PREFIX_PATH=$INSTALL_PATH/include/SGR:\${CMAKE_PREFIX_PATH}" $USER_CONFIG_FILE; then
			echo >> $USER_CONFIG_FILE
			echo "export CMAKE_PREFIX_PATH=$INSTALL_PATH/include/SGR:\${CMAKE_PREFIX_PATH} #for cmake package_find command" >> $USER_CONFIG_FILE
		fi
		
		if ! grep -q "export SGR_LIB=$INSTALL_PATH/lib" $USER_CONFIG_FILE; then
			echo "export SGR_LIB=$INSTALL_PATH/lib/" >> $USER_CONFIG_FILE
			echo "SDK environement added to user config file successfully."
		fi
	fi

	cd Release
	rm -rf *.tar
	for entry in `ls $search_dir`; do
		tar -cf $entry.tar $entry
	done
	cd ..
fi

cd ..


# Example build and run if requested
if [ $EXAMPLE_BUILD == true ]
then
	./build/exampleApplication/SGR_test
fi

############################################################