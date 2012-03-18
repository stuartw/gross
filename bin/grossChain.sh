#!/bin/bash

# script to chain output root files from gross together.
usage() {
  echo "Usage: `basename $0` -t <gross taskId> -r <root tree name> [-f <outputFile> -j <gross jobId range>]"
}

parseGross() {
  while read line; do
    dir=`echo "$line" | awk '{ print $3 }'`
    if [ ${dir} != "_SBOX_RETRIEVAL_ERROR_" ] && [ ${dir} != "GROSS" ]; then
      dirs="$dirs ${dir}"
    fi
  done
}
export taskId="0"
export jobId=""
export file="gross.root"
export tree="T1"
export dirs=""
export rootFiles=""
tempFile="/tmp/gross.$$.tmp"

while getopts t:f:j:r: o; do
  case "$o" in
    t) taskId=${OPTARG};;
    j) jobId="-jobId ${OPTARG}";;
    f) file=${OPTARG};;
    r) tree=${OPTARG};; 
  esac
done

if [ ${taskId} -eq 0 ]; then
  usage
  exit 1
fi

# run gross and pass output finding output dirs
command="gross o -taskId ${taskId} ${jobId}"
${command} > ${tempFile}
parseGross < ${tempFile}

for dir in ${dirs}; do
  if [ -d ${dir} ]; then
    ls ${dir}/*.root.* 2>&1 > /dev/null    
    if [ $? -eq 0 ]; then
      rootFiles=" ${rootFiles} `ls ${dir}/*.root.*`"
    else
      echo "root file not found. continuing without it"
    fi
  fi
done

#echo ${dirs}
#echo ${rootFiles}

#clear temp file and dump root macro to it
rm ${tempFile}
cat >${tempFile} <<EOF
{
TChain chain("${tree}");
EOF
for root in ${rootFiles}; do
  echo "chain.Add(\"${root}\");" >> ${tempFile}
done
echo "chain.Merge(\"${file}\");" >> ${tempFile}
echo "}" >> ${tempFile}

root -q -b ${tempFile}

#rm -f ${tempFile}

if [ $? -ne 0 ]; then
  echo "Error: root files not chained together - if 1 file is problematic try deleting it or moving it out of the output dir in the Db"
  exit 1
fi

exit 0
