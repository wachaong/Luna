#!/bin/bash

cd ../

export PATH=/home/a/share/algo1/openmpi-current/bin:$PATH
export LD_LIBRARY_PATH=.:/home/a/share/algo1/openmpi-current/lib/:/home/a/lib64/:/usr/local/lib:${LD_LIBRARY_PATH}
export JAVA_HOME=/usr/java/default
/home/ads/tools/apache-ant-1.7.1/bin/ant

make
if [ $? != 0 ]
then
  echo "Failed make"
  exit -1
fi

cd rpm
/usr/local/bin/rpm_create -p /home/a/share/algo1/mpi-lr -v $3 -r $4 $2.spec -k
