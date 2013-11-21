#!/bin/sh

DES_FILE=`printf ins-%05d ${OMPI_COMM_WORLD_RANK}`
SRC_FILE=`printf ins-%05d ${OMPI_COMM_WORLD_RANK}`
mv $1/$SRC_FILE $2/$DES_FILE

