#!/bin/bash

if [ $# -lt 3 ]
then
	echo "Usage: mergeInsScore.sh insPathPrefix scorePath outfilename"
	exit 1
fi

rankStr=`printf "%05d" ${OMPI_COMM_WORLD_RANK}`

rankDir="rank-${rankStr}"

mkdir -p ${rankDir}	

outPath="${rankDir}/$3"

insPath="${1}-${rankStr}"

paste -d $'\001' "$2"  "$insPath"  > "$outPath"
