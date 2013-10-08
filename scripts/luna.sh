#!/bin/bash
for ((i=21;i<=30;i++)); do
	done_file=/group/tbalgo-dev/yanling.yl/Luna/1.0.0/DataFilter/output/201309$i/step1.done
	job_name=DataFilter_201309$i
	if [ -f $done_file ];then
    	echo "$done_file exist"
	else
		echo "Job: $job_name starting"
    	bash run.sh DataFilter 201309$i &
	fi
done


