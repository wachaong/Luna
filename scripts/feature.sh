#!/bin/bash

for ((i=20;i<=30;i++)); do
	job_name=FeatureSub_201309$i
	echo "Job: $job_name starting"
    bash ~/Luna/scripts/featureSub.sh FeatureMapper 201309$i &
done

for ((i=1;i<10;i++)); do
	job_name=FeatureSub_2013100$i
	echo "Job: $job_name starting"
   bash ~/Luna/scripts/featureSub.sh FeatureMapper 2013100$i &
done
job_name=FeatureSub_20131010
echo "Job: $job_name starting"
bash ~/Luna/scripts/featureSub.sh FeatureMapper 20131010 &

wait
echo "Feature Sub Done"


set -eu
application_home=$(readlink -f $(dirname $0))/..
work_dir=$application_home
conf_dir=$application_home/conf

gmtdate=$(date +%Y%m%d)
cmpdate=$(date -d "$gmtdate -1 day" +%Y%m%d)


#[[ $# == 3 ]] || usage
version=1.0.1
flow=MergeFeature
end_date=20131010
DATE=20131010
NEXTDATE=20131010
num_days=21
begin_date=$(date -d "$end_date -$num_days day" +%Y%m%d)

source $conf_dir/application.conf
featuremap=$data_dir/featureMap.txt

# set classpath
classpath=$conf_dir
for jar in $lib_dir/*.jar; do classpath=$classpath:$jar; done

source $application_home/scripts/hadoop.rc
source $application_home/scripts/main.rc

input_path=$(get_input_path $end_date $num_days FeatureMap step0)
echo $input_path
output=/group/tbalgo-dev/yanling.yl/Luna/${version}/FeatureMap/output/step0/part*
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/FeatureMap/output/step0
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/FeatureMap/output/step0.busy
hrmr /group/tbalgo-dev/yanling.yl/Luna/${version}/FeatureMap/output/step0.counter



HADOHADOOP_HEAPSIZE=4000 HADOOP_CLASSPATH=$classpath \
    $hadoop_exec --config $hadoop_exec_conf \
    jar $husky_jar com.taobao.husky.flow.Launcher \
    -D application.home=$application_home \
    -D gmtdate=$gmtdate \
    -D cmpdate=$cmpdate \
    -D USER=$USER \
    -D mapred.input.dir=$input_path \
    ${properties[@]-} \
    $flow

#-files $shop_cate\
#-D shop.cate=`basename $shop_cate` \

hdfs_featuremap=/group/tbalgo-dev/yanling.yl/Luna/${version}/featureMap.txt
hrmr $hdfs_featuremap
rm -rf $featuremap
hadoop fs -cat $output  > $featuremap
hadoop fs -put $featuremap $hdfs_featuremap
