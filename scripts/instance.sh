#!/bin/bash

set -eu
application_home=$(readlink -f $(dirname $0))/..
work_dir=$application_home
conf_dir=$application_home/conf

function usage() {
    echo "Usage: $0 FLOW TRAIN_END_DATE TRAIN_DAYS TEST_END_DATE TEST_DAYS [-c CONF] [-t GMTDATE] [-q QUEUE] \
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


[[ $# == 5 ]] || usage
flow=$1
end_date_train=$2
end_date_test=$4
DATE=$4
NEXTDATE=$4
num_days_train=$3
num_days_test=$5
begin_date_train=$(date -d "$end_date_train -$num_days_train day" +%Y%m%d)
begin_date_test=$(date -d "$end_date_test -$num_days_test day" +%Y%m%d)
source $conf_dir/application.conf


featuremap=$data_dir/featureMap.txt

# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done

source $application_home/scripts/hadoop.rc
source $application_home/scripts/main.rc


input_path_train=$(get_input_path $end_date_train $num_days_train)
input_path_test=$(get_input_path $end_date_test $num_days_test)



hrmr /group/tbalgo-dev/yanling.yl/Luna/1.0.0/Instance/

HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -files $featuremap \
    -D feature.map=`basename $featuremap` \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D train.input=$input_path_train \
    -D test.input=$input_path_test \
    ${properties[@]-} \
    $flow
    

