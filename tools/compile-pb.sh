#!/bin/bash

work_dir=$(readlink -f $(dirname $0))/..

function usage() {
    echo "Usage: $0 [-v PROTOBUF_VERSION] [-h]" >&2
    exit $1
}

pb_version=2.5.0
while getopts "v:h" opt; do
    case $opt in
        v) pb_version=$OPTARG;;
        h) usage 0;;
        \?) echo "Invalid option: -$OPTARG" >&2; usage 1;;
    esac
done

protoc_dir=$work_dir/tools/protoc/$(uname)/$pb_version
protoc=$protoc_dir/protoc
export LD_LIBRARY_PATH=$protoc_dir
export DYLD_LIBRARY_PATH=$protoc_dir
pb_dir=$work_dir/src/proto
pb_files=($pb_dir/*.proto)
java_out=$work_dir/src/main/java
mkdir -p $java_out
for f in ${pb_files[@]}; do
    $protoc -I $pb_dir --java_out=$java_out $f
done


