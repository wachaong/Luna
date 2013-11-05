#!/bin/bash
usage(){
	echo "USAGE: $0 name"
	exit $1
}

[[ $# = 1 ]] || usage
NAME=$1

WORK_DIR=`readlink -f $0`
WORK_DIR=`dirname $WORK_DIR`
WORK_DIR=`cd $WORK_DIR/../; pwd`

BIN_DIR="$WORK_DIR/bin"
CONF_DIR="$WORK_DIR/conf"
COMMON_DIR="$WORK_DIR/common"
LIB_DIR="$WORK_DIR/jar"
ts=`date +%s`

HADOOP_CONF_DIR=/home/yanling.yl/conf

function FeatureSign() {
  if [ $# -ne 6 ]
  then
    echo "FeatureSign input_dir output_dir mapper_num reducer_num is_seq pv_threshold"
    return -1
  fi
  local input_dir="$1"
  local output_dir="$2"
  local mapper_num="$3"
  local reducer_num="$4"
  local is_seq="$5"
  local pv_threshold="$6"
  local is_fea_filter="0"
  if [ $pv_threshold -gt 0 ]
  then
    is_fea_filter=1
  fi
  if [ -z $input_dir -o -z $output_dir -o -z $mapper_num -o -z $reducer_num -o -z $is_seq -o -z $pv_threshold ]
  then
    echo "FeatureSign input_dir output_dir mapper_num reducer_num is_seq pv_threshold"
    return -1
  fi
  output_dir_tmp=${output_dir}/tmp
  feature_dir=${output_dir}/feature
  instance_dir=${output_dir}/instance

  log_info "FeatureSign start";
 
  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} jar -files ${CONF_DIR}/log4j.xml \
     ${LIB_DIR}/Luna.jar com.alimama.loganalyzer.common.Launcher com.taobao.mpi.algo.FeatureSign \
     $input_dir $output_dir_tmp $mapper_num $reducer_num $is_seq pv=$pv_threshold isFeaFilter=$is_fea_filter
  if [ $? -ne 0 ]
  then
    log_error "FeatureSign failed!";
    return 1;
  else
    log_info "FeatureSign success!";
  fi

  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -rmr $feature_dir
  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -mkdir $feature_dir
  if [ $? != 0 ]
  then
    log_error "hadoop --config ${HADOOP_CONF_DIR} fs -mkdir ${feature_dir} failed"
    exit -1
  fi
  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -mv $output_dir_tmp/fea* ${feature_dir}
  if [ $? != 0 ]
  then
    log_error "hadoop --config ${HADOOP_CONF_DIR} fs -mv $output_dir_tmp/fea* ${feature_dir}"
    exit -1
  fi

  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -rmr $instance_dir
  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -mkdir $instance_dir
  if [ $? != 0 ]
  then
    log_error "hadoop --config ${HADOOP_CONF_DIR} fs -mkdir ${instance_dir} failed"
    exit -1
  fi
  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -mv $output_dir_tmp/ins* ${instance_dir}
  if [ $? != 0 ]
  then
    log_error "hadoop --config ${HADOOP_CONF_DIR} fs -mv $output_dir_tmp/ins* ${instance_dir}"
    exit -1
  fi
  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} fs -rmr $output_dir_tmp
  if [ $? != 0 ]
  then
    log_error "hadoop --config ${HADOOP_CONF_DIR} fs -rmr $output_dir_tmp"
    exit -1
  fi
  return 0;
}

function FeatureFilter() {
  if [ $# -ne 6 ]
  then
    echo "FeatureFilter input_dir output_dir mapper_num reducer_num is_seq pv_threshold"
    return -1
  fi
  local input_dir="$1"
  local output_dir="$2"
  local mapper_num="$3"
  local reducer_num="$4"
  local is_seq="$5"
  local pv_threshold="$6"
  if [ -z $input_dir -o -z $output_dir -o -z $mapper_num -o -z $reducer_num -o -z $is_seq -o -z $pv_threshold ]
  then
    echo "FeatureFilter input_dir output_dir mapper_num reducer_num is_seq pv_threshold"
    return -1
  fi

  log_info "FeatureFilter start";

  ${HADOOP_HOME}/bin/hadoop --config ${HADOOP_CONF_DIR} jar -files ${CONF_DIR}/log4j.xml \
     ${LIB_DIR}/Luna.jar com.alimama.loganalyzer.common.Launcher com.taobao.mpi.algo.FeaFilter \
     $input_dir $output_dir $mapper_num $reducer_num $is_seq pv=$pv_threshold
  if [ $? -ne 0 ]
  then
    log_error "FeatureFilter failed!";
    return 1;
  else
    log_info "FeatureFilter success!";
  fi

  return 0;
}


HDHOME=/group/tbalgo-dev/yanling.yl
inputs=${HDHOME}/Luna/1.0.1/${NAME}/output/train/part*
evaluate_inputs=${HDHOME}/Luna/1.0.1/${NAME}/output/test/part*


HADOOP_MODEL_PATH=${HDHOME}/Luna/1.0.1/Modelv2
HADOOP_OUT_PATH=${HADOOP_MODEL_PATH}/LR_JOB_${NAME}
hadoop fs -rmr $HADOOP_OUT_PATH

output=${HADOOP_OUT_PATH}/model
VALID_FEATURE_HADOOP_PATH=${output}/valid
FEATURE_HADOOP_PATH=${output}/feature
TEST_INSTANCE_HADOOP_PATH=${output}/test/instance

INPUT_SIGN_MAPPER_NUM=100
INPUT_SIGN_REDUCER_NUM=100
INPUT_IS_SEQ=false
PV_THRESHOLD=0
reducer_num=$INPUT_SIGN_REDUCER_NUM

#build feature dict

FeatureSign ${inputs} ${output} ${INPUT_SIGN_MAPPER_NUM} ${reducer_num} ${INPUT_IS_SEQ} ${PV_THRESHOLD}
FeatureSign ${evaluate_inputs} ${output}/test ${INPUT_SIGN_MAPPER_NUM} ${reducer_num} ${INPUT_IS_SEQ} ${PV_THRESHOLD}
FeatureFilter ${FEATURE_HADOOP_PATH} ${VALID_FEATURE_HADOOP_PATH} ${INPUT_SIGN_MAPPER_NUM} 1 false ${PV_THRESHOLD}

echo "Done"



