#!/bin/sh

if [ $# -ne 2 ]
then
  echo "Usage $0 src dest" >&2
  exit -1
fi
SRC_FILE=`printf ins-%05d ${OMPI_COMM_WORLD_RANK}`
DES_FILE=`printf ins-%05d ${OMPI_COMM_WORLD_RANK}`
mv $1/$SRC_FILE $2/$DES_FILE
