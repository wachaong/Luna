#!/bin/bash

cat $1  | awk -F"" '{print $4}' | sort -u > $2

