#!/bin/bash

set -eu
application_home=$(readlink -f $(dirname $0))/..


function usage() {
    echo "Usage: $0 FLOW DATE [-c CONF] [-t GMTDATE] [-q QUEUE] \
	[-D NAME=VALUE] [-h] NAME" >&2
    exit $1
}


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

[[ $# == 2 ]] || usage
flow=$1
DATE=$2

source $conf_dir/application.conf



# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done

set +e
hadoop fs -rmr /group/tbalgo-dev/yanling.yl/Luna/1.0.0/${flow}/output/${DATE}
mkdir -p `dirname $customer_file`
rm -rf $customer_file
htext $hdfs_customer/*  | perl  -lane 's/\s+//;next if /\cA\\N\cA/ ;split /\cA/ ; print "$_[0]\cA$_[18]"' > $customer_file


HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -files $train_pid.conf,$customer_cate\
    -D application.home=$application_home \
    -D out_dir=$out_dir \
    -D train_pid=`basename $train_pid` \
    -D customer_cate=`basename $customer_cate` \
    -D midlog_input=$midlog_input \
    -D midad_input=$midad_input \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    ${properties[@]-} \
    $flow
