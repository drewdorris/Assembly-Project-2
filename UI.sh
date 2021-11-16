#!/bin/bash
############################################
## scriptname.sh
## 	Author:		Charles
## 	Date: 		11/9/2021
## 	Last revised:	11/9/2021
## 	Purpose: 	Interactive script
## 			to display text file info
##
############################################

DEFAULT="\e[39m"
RED="\e[31m"
BLUE="\e[34m"
GREEN="\e[32m"
PURPLE="\e[35m"
CYAN="\e[36m"
H_LINE="================================="

#variables
count=1
fileIn=false
stringIn=""
fileOut=false

###########
#functions#
###########
#header for each display
function header() {
	clear
	printf $GREEN" $1\n"
	printf "%s\n"$DEFAULT "$HORIONTAL_LINE"
}
#pause and wait for user to press enter
function pause() {
	read -p "Press Enter to continue..."
	clear
}



#main loop
while true
do
	clear
	printf $GREEN"\tMENU\n"
	printf "%s\n" "$H_LINE"

	#Check status of input and display
	if [ $fileIn = true && stringIn = ""]; then
		printf $RED"!! Currently no input string\n"$GREEN
	fi
	#Check input type and display
	if [ $fileIn = true ]; then
		printf "# Input Type: File\n"
	else
		printf "# Input Type"

	printf "%s\n" "$H_LINE"
	printf "# 1. Add Terminal Input\n"
	printf "# 2. Add File Input\n"
	printf "# 3. Toggle File Output\n"
	printf "# 4. Toggle Terminal Output\n"
	printf "# 5. Exit\n"
    printf "%s\n"$DEFAULT "$H_LINE"

	read -p "Enter selection: "

	case $REPLY in
		1) header " Displaying $1\n"
		   display_all $1
		   ;;
                2) header " Displaying Summary\n"
		   display_summary $1
		   ;;
                3) header " # of Lines\n"
                   display_lines $1
                   ;;
                4) header " # of Words\n"
                   display_words $1
                   ;;
                5)clear
		  printf "So long and thanks for all the fish!"
		  sleep 1s
		  clear 
		  break;;
	esac
done

exit 0
