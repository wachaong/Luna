#!/bin/bash
for ((i=20;i<=30;i++)); do
	job_name=DataFilter_201309$i
	echo "Job: $job_name starting"
    bash ~/Luna/scripts/filter.sh DataFilter 201309$i &
done

for ((i=1;i<10;i++)); do
	job_name=DataFilter_2013100$i
	echo "Job: $job_name starting"
    bash ~/Luna/scripts/filter.sh DataFilter 2013100$i &
done
job_name=DataFilter_20131010
echo "Job: $job_name starting"
bash ~/Luna/scripts/filter.sh DataFilter 20131010

#touch ~/Luna/data/201309$i/job.done
java -jar jar/Luna.jar
hadoop fs -put ~/Luna/data/shop2cate.txt /group/tbalgo-dev/yanling.yl/Luna/1.0.1/shop2cate.txt

#bash ~/Luna/scripts/feature.sh FeatureMapper 20130930 21

#usage(){
#    echo "Usage: $0 NAME"
#    exit $1
#}
#[[ $# == 1 ]] || usage 
#name=$1
#rm ~/Luna/data/featureMap.txt
#hadoop fs -rmr /group/tbalgo-dev/yanling.yl/Luna/1.0.0/$name
#bash ~/Luna/scripts/feature.sh FeatureMapper 20130930 21
#hadoop fs -mkdir /group/tbalgo-dev/yanling.yl/Luna/1.0.1/$name
#hadoop fs -mv /group/tbalgo-dev/yanling.yl/Luna/1.0.0/FeatureMap /group/tbalgo-dev/yanling.yl/Luna/1.0.0/$name
#hadoop fs -put ~/Luna/data/featureMap.txt /group/tbalgo-dev/yanling.yl/Luna/1.0.0/$name
#bash ~/Luna/scripts/instance.sh Instance 20130923 14 20130930 7
#hadoop fs -mv /group/tbalgo-dev/yanling.yl/Luna/1.0.0/Instance /group/tbalgo-dev/yanling.yl/Luna/1.0.0/$name

