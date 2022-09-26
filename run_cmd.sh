#!/bin/sh

# remove the log file & game binary
rm log.txt
rm game

usage() {

	echo "${0} check the directory"
	echo "It should have the binary 'game' in the" 
	echo "same directory"
}

#compile the code with make command
make

if [ $? -ne 0 ] 
then
	usage
	exit 1
fi

#execute the cmd to start the game
./game
