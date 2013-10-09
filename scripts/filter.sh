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
DATE=$2
NEXTDATE=$(date -d "$DATE +1 day" +%Y%m%d)
source $conf_dir/application.conf

shop_input="/group/taobao/taobao/hive/r_seller_basic_info/pt="$DATE"000000"
shop_output=/group/tbalgo-dev/yanling.yl/Luna/1.0.0/${flow}/output/${DATE}/step0/part-r-*


# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done


[[ -f ~/Luna/data/${DATE}/job.done ]] && exit $1

set +e
#hadoop fs -rmr /group/tbalgo-dev/yanling.yl/Luna/1.0.0/${flow}/output/${DATE}
hadoop fs -rmr /group/tbalgo-dev/yanling.yl/Luna/1.0.0/${flow}/output/${DATE}/step1
hadoop fs -rm /group/tbalgo-dev/yanling.yl/Luna/1.0.0/${flow}/output/${DATE}/step1.*

#if [ -f $customer_cate ];then
#    echo "$customer_cate exist"
#else
#    mkdir -p `dirname $customer_cate`
#    rm -rf $customer_cate
#    hadoop fs -text $hdfs_customer/*  | perl  -lane 's/\s+//;next if /\cA\\N\cA/ ;split /\cA/ ; print "$_[0]\cA$_[18]"' > $customer_cate
#fi


HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -files $train_pid,$customer_cate\
    -D application.home=$application_home \
    -D train.pid=`basename $train_pid` \
    -D customer.cate=`basename $customer_cate` \
    -D midlog.input=$midlog_input \
    -D midad.input=$midad_input \
    -D midad.input2=$midad_input2 \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D DATE=$DATE \
    -D NEXTDATE=$NEXTDATE \
    -D shop.input=$shop_input \
    -D shop.output=$shop_output \
    ${properties[@]-} \
    $flow
    
#hadoop fs -cat $shop_output > $shop_cate
