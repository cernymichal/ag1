#!/bin/bash

<<help

-q - quiet mode
-a - show passing tests' output as well 
-s - shallow mode
-d - show diff instead of output
-t=directory - specify directcory with tests (default ./tests)
-x=executable - specify executable to test on (default ./main)

help

NC='\033[0m'
RED_BG='\033[41m'
GREEN_BG='\033[42m'
WBOLD='\033[1;37m'
CBOLD='\033[1;36m'
WUNDER='\033[4;37m'
L_BRACKET="${CBOLD}[${NC}"
R_BRACKET="${CBOLD}]${NC}"
SUCCESS="${L_BRACKET}${WBOLD}${GREEN_BG}V${NC}${R_BRACKET}"
FAIL="${L_BRACKET}${WBOLD}${RED_BG}X${NC}${R_BRACKET}"
INDENT="    "

EXIT=0

QUIET=false
ALL_OUTPUT=false
SHALLOW=false
SHOW_DIFF=false
TESTS_DIR='./tests'
EXECUTABLE='./main'

while getopts 'qasdt:x:' flag
do
    case "${flag}" in
        q)
            QUIET=true
            ;;
        a)
            ALL_OUTPUT=true
            ;;
        s)
            SHALLOW=true
            ;;
        d)
            SHOW_DIFF=true
            ;;
        t)
            TESTS_DIR=${OPTARG}
            ;;
        x)
            EXECUTABLE=${OPTARG}
            ;;
    esac
done

EXECUTABLE="$(cd "$(dirname "$EXECUTABLE")" && pwd -P)/$(basename "$EXECUTABLE")"
TESTS_DIR="$(cd "$(dirname "$TESTS_DIR")" && pwd -P)"

TESTS_DIRS=( "$TESTS_DIR" )

if ! $SHALLOW
then
    TESTS_DIRS+=( "$TESTS_DIR"/*/ )
fi

cd "$(dirname "$EXECUTABLE")"

for directory in "${TESTS_DIRS[@]}"
do
    if ! $(ls "$directory"/*_in.txt 2>/dev/null >/dev/null)
    then
        continue
    fi

    printf "${WBOLD}$directory${NC}\n"

    for in_file in "$directory"/*_in.txt
    do
        output=$("$EXECUTABLE" < "$in_file")
        num=$(echo "$in_file" | sed 's/_in.txt//')
        out_file="${num}_out.txt"
        num=$(echo "$num" | grep -o '....$')
        if ! $(echo "$output" | diff "$out_file" - > /dev/null)
        then
            EXIT=1
            printf "$FAIL $num\n"
            
            if ! $QUIET
            then
                if $SHOW_DIFF
                then
                    printf "${WUNDER}Diff:${NC}\n$(echo "$output" | diff -u --color=always "$out_file" - | tail --lines=+3)\n" | sed "s/^/$INDENT/"
                else
                    printf "${WUNDER}Input:${NC}\n$(cat "$in_file")\n${WUNDER}Expected output:${NC}\n$(cat "$out_file")\n${WUNDER}Actual output:${NC}\n$output\n" | sed "s/^/$INDENT/"
                fi
            fi
        else
            printf "$SUCCESS $num\n"

            if $ALL_OUTPUT && ! $QUIET
            then
                if $SHOW_DIFF
                then
                    printf "${WUNDER}Diff:${NC}\n$(echo "$output" | diff -u --color=always "$out_file" - | tail --lines=+3)\n" | sed "s/^/$INDENT/"
                else
                    printf "${WUNDER}Input:${NC}\n$(cat "$in_file")\n${WUNDER}Output:${NC}\n$output\n" | sed "s/^/$INDENT/"
                fi
            fi
        fi
    done
done

exit "$EXIT"
