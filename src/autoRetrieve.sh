#!/bin/bash

# script to retrieve jobs submitted to LCG2

#commandLine  autoRetrieve 1 'pwd'

if [ $# -lt 2 ]; then
  echo "Usage: " `basename $0` "taskId outputDir"
  exit 1
fi

TASK=$1
OUTPUT=$2 
LOG=${OUTPUT}/gross_task_${TASK}_retrieve.log
INTERVAL=60 #time between retrievals in seconds
JOBSLEFT=0
LENGTH=360 #number of time to run multiply by $INTERVAL to get time script will run for
NOW=${LENGTH}

if [ -f $LOG ]; then
  rm -f $LOG
fi
touch $LOG

echo "===> Starting GROSS auto retrieval of task $TASK on" `hostname` "at" `date` >> $LOG

while [ 1 ]
do
  #out=`
  gross o -taskId $TASK -oDir $OUTPUT -cont &> /dev/null 
  #`
  JOBSLEFT=$?
  
  if [ $JOBSLEFT -eq 0 ]; then
    break
  else
    echo `date`": ${JOBSLEFT} Job(s) output currently unavailable - will retry again in $INTERVAL seconds time" >> $LOG
  fi
 
  NOW=`echo "$NOW - 1" | bc`
  if [ $NOW -le 0 ];then
    echo "===> Aborting retievals at "`date` >> $LOG
    exit 1
  else
    sleep $INTERVAL
  fi

done

echo "===> Output retrieval finished at "`date` >> $LOG


