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
HORIZONTAL_LINE="================================="

## Error checking  ####################
if [[ $# -ne 1 ]]; then
  echo -e $CYAN" Usage: fileinfo.sh input_file"$DEFAULT
  exit 1
fi
## check file type
type=$(file -b $1)
if [[ $type != "ASCII text" ]]; then
	echo -e $CYAN"Error! $1 is not a text file."
	exit 1
fi
#########################################

#variables
count=1
lines=$(wc $1 | tr -s ' ' | cut -d' ' -f2)
words=$(wc $1 | tr -s ' ' | cut -d' ' -f3)
characters=$(wc $1 | tr -s ' ' | cut -d' ' -f4)

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
	printf "%s\n" "$HORIZONTAL_LINE"
	printf "# 1. Display all \n"
	printf "# 2. Display Summary Info\n"
	printf "# 3. # of Lines\n"
	printf "# 4. # of Words\n"
 	printf "# 5. # of Characters\n"
	printf "# 6. Exit\n"
        printf "%s\n"$DEFAULT "$HORIZONTAL_LINE"

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
                5) header " # of Characters\n"
                   display_characters $1
                   ;;
                6)clear
		  printf "So long and thanks for all the fish!"
		  sleep 1s
		  clear 
		  break;;
	esac
done

exit 0
