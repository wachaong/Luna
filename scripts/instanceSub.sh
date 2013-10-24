#!/bin/bash

set -eu
application_home=$(readlink -f $(dirname $0))/..
work_dir=$application_home
conf_dir=$application_home/conf

function usage() {
    echo "Usage: $0 FLOW DATE [-c CONF] [-t GMTDATE] [-q QUEUE] \
	[-D NAME=VALUE] [-h] NAME" >&2
    exit $1
}


gmtdate=$(date +%Y%m%d)
properties=()
cmpdate=$(date -d "$gmtdate -1 day" +%Y%m%d)


[[ $# == 2 ]] || usage
flow=$1
version=1.0.1
DATE=$2
NEXTDATE=$2
source $conf_dir/application.conf


featuremap=$data_dir/featureMap.txt

# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done

source $application_home/scripts/hadoop.rc
source $application_home/scripts/main.rc


input_path=/group/tbalgo-dev/yanling.yl/Luna/1.0.1/DataFilter/output/$DATE/step1
output_path=/group/tbalgo-dev/yanling.yl/Luna/${version}/Instance/output/$DATE/step0
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/Instance/output/$DATE/


HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -files $crowdpower \
    -D crowd.power=`basename $crowdpower` \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D mapred.input.dir=$input_path \
    -D mapred.output.dir=$output_path \
    ${properties[@]-} \
    $flow
    

