#!/bin/bash

set -eu
application_home=$(readlink -f $(dirname $0))/..
work_dir=$application_home
conf_dir=$application_home/conf

#for ((i=28;i<=30;i++)); do
#	job_name=CF_201309$i
#	echo "Job: $job_name starting"
#    bash ~/Luna/scripts/cf.sh CF 201309$i &
#done

#for ((i=1;i<9;i++)); do
#	job_name=CF_2013100$i
#	echo "Job: $job_name starting"
#   bash ~/Luna/scripts/cf.sh CF 2013100$i &
#done

#wait
#echo "Data CF Done"

flow=Stats
version=1.0.1
end_date_train=20131007
num_days_train=4
DATE=$end_date_train
NEXTDATE=$end_date_train
gmtdate=$(date +%Y%m%d)
cmpdate=$(date -d "$gmtdate -1 day" +%Y%m%d)

begin_date_train=$(date -d "$end_date_train -$num_days_train day" +%Y%m%d)
source $conf_dir/application.conf

classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done

source $application_home/scripts/hadoop.rc
source $application_home/scripts/main.rc


input_path_train_ad4=$(get_input_path $end_date_train 4 CF Ad)
input_path_train_newuser4=$(get_input_path $end_date_train 4 CF NewUser)
input_path_train_user4=$(get_input_path $end_date_train 4 CF User)

input_path_train7=$(get_input_path $end_date_train 7 CF step0)
input_path_train_newuser7=$(get_input_path $end_date_train 7 CF NewUser)
input_path_train_user7=$(get_input_path $end_date_train 7 CF User)

input_path_train10=$(get_input_path $end_date_train 10 CF step0)
input_path_train_newuser10=$(get_input_path $end_date_train 10 CF NewUser)
input_path_train_user10=$(get_input_path $end_date_train 10 CF User)

output_path4=/group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/Stat4
output_path7=/group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/Stat7
output_path10=/group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/Stat10

hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/stat4
hadoop fs -rm /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/stat4.*
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/stat7
hadoop fs -rm /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/stat7.*
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/stat10
hadoop fs -rm /group/tbalgo-dev/yanling.yl/Luna/${version}/CF/output/stat10.*


HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D train.input.ad=$input_path_train_ad4 \
    -D train.input.newuser=$input_path_train_newuser4 \
    -D train.input.user=$input_path_train_user4 \
    -D output.dir=$output_path4 \
    ${properties[@]-} \
    $flow
    
HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D train.input.ad=$input_path_train_ad7 \
    -D train.input.newuser=$input_path_train_newuser7 \
    -D train.input.user=$input_path_train_user7 \
    -D output.dir=$output_path7 \
    ${properties[@]-} \
    $flow
    
HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D train.input.ad=$input_path_train_ad10 \
    -D train.input.newuser=$input_path_train_newuser10 \
    -D train.input.user=$input_path_train_user10 \
    -D output.dir=$output_path10 \
    ${properties[@]-} \
    $flow
    
    