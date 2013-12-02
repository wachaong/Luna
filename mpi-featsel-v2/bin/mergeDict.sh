#!/bin/bash

if [ $# -ne 2 ]
then
  echo "Usage $0 src dest" >&2
  exit -1
fi

SrcDir=$1
DesFile=$2

cat $SrcDir/* >>$DesFile
