#!/bin/bash
# Check if an include file exists
# syntax: cpp mysql/mysql.h [-I/usr/include]

CPP=$1;	shift
INC=$1; shift
FLAGS=$*
${CPP} ${FLAGS} << EOF > /dev/null 2>&1
#include <${INC}>
EOF
RC=$?
echo $RC
exit $RC
