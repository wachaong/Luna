#!/bin/bash

set -eu
application_home=$(readlink -f $(dirname $0))/..
conf_dir=$application_home/conf

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
end_date=$1
num_days=11
begin_date=$(date -d "$end_date -$num_days day" +%Y%m%d)

source $conf_dir/application.conf


featuremap=$data_dir/featureMap.txt

# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done


if [ -f $output ];then
    echo "$output exist"
else
    rm -rf $output
fi

input_path=$(get_input_path $end_date $num_days $daily_output $output)
output=/group/tbalgo-dev/yanling.yl/Luna/1.0.0/${flow}/output/step0/part*

HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D mapred.input.dir=$input \
    ${properties[@]-} \
    $flow
    
hadoop fs -cat $output > $featuremap
