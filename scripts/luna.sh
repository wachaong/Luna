#!/bin/bash
#for ((i=10;i<20;i++)); do
#	job_name=DataFilter_201309$i
#	echo "Job: $job_name starting"
#   bash ~/Luna/scripts/filter.sh DataFilter 201309$i
#   touch ~/Luna/data/201309$i/job.done
#   java -jar jar/Luna.jar
#done
bash ~/Luna/scripts/feature.sh FeatureMapper 20130930 21
bash ~/Luna/scripts/instance.sh Instance 20130923 14 20130930 7


