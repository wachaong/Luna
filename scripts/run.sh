#!/bin/bash

set -eu
application_home=$(readlink -f $(dirname $0))/..
application_lib=$application_home/lib

function usage() {
    echo "Usage: $0 [-c CONF] [-t GMTDATE] [-q QUEUE] \
[-D NAME=VALUE] [-h] NAME" >&2
    exit $1
}

conf=$application_home/conf
gmtdate=$(date +%Y%m%d)
properties=()
while getopts "c:t:q:D:h" opt; do
    case $opt in
        c) conf=$OPTARG;;
        t) gmtdate=$OPTARG;;
        q) properties=(${properties[@]-} "-D" "mapred.job.queue.name=$OPTARG");;
        D) properties=(${properties[@]-} "-D" "$OPTARG");;
        h) usage 0;;
        \?) echo "Invalid option: -$OPTARG" >&2; usage 1;;
    esac
done
shift $((OPTIND-1))
cmpdate=$(date -d "$gmtdate -1 day" +%Y%m%d)

[[ $# == 1 ]] || usage 1
flow=$1

source $conf/application.conf

# set classpath
classpath=$conf 
for jar in $application_lib/*.jar; do classpath=$classpath:$jar; done

hadoop fs -rmr /group/tbalgo-dev/yanling.yl/Luna/1.0.0/Testfilter/output/${gmtdate}

HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -files $conf/train_pid.conf \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D filter.pid.file=train_pid.conf \
    ${properties[@]-} \
    $flow
