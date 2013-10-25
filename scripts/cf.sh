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
version=1.0.1
flow=$1
end_date=$2
DATE=$2
NEXTDATE=$2
num_days=1

source $conf_dir/application.conf

# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done

source $application_home/scripts/hadoop.rc
source $application_home/scripts/main.rc

input_path=/group/tbalgo-dev/yanling.yl/Luna/${version}/DataFilter/output/${DATE}/step1
output_path=/group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/${DATE}
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/${DATE}


HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D mapred.input.dir=$input_path \
    -D output.dir=$output_path \
    ${properties[@]-} \
    $flow

