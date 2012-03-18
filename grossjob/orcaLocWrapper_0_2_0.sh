#!/bin/bash
#set -x

#Functions for producing BOSS viewable monitoring info
printERROR() {
  echo "wrapper:ERROR: " $@>&1
}

printWARNING() {
  echo "wrapper:WARNING: " $@>&1
}

printCHECKPOINT() {
  echo "wrapper:CHECKPOINT: " $@>&1
}

printINFO() {
  echo "wrapper:INFO: " $@>&1
}


checkVariable() {
  variable=$1
  defined=`env | grep -c ${variable}`
  if [ ${defined} -eq 0 ]; then
    printERROR "${variable} not defined"
  fi
}

#Start of script
printCHECKPOINT "Wrapper script starting..."

outDir=$1
STEERFILE="$0.steer"

#Check steering file exists
if ! [ -a ${STEERFILE} ]; then
    printERROR "Steering file ${STEERFILE} not found"
    exit 1
fi

#Set variables from seering file
printCHECKPOINT "Reading steering file"
extvble=`cat ${STEERFILE} | awk '{print $1}' | uniq`
for vble in ${extvble}; do
  comm=`grep ${vble} ${STEERFILE} | awk '{print $2}'`
  export $vble="$comm"
  printINFO "Setting variable from external steering file:" ${vble} ${comm[*]}
done

#check for variables from  steering file, exit if not found
printCHECKPOINT "Checking variables set from ${STEERFILE}..."
echo ${DarSetupFile?"ERROR ! Variable not defined"} > /dev/null
echo ${Suffix?"ERROR ! Variable not defined"} > /dev/null
echo ${userExec?"ERROR ! Variable not defined"} > /dev/null
echo ${XMLFragFile?"ERROR ! Variable  not defined"} > /dev/null

STARTDIR=`pwd`
STARTLD=$LD_LIBRARY_PATH

#Set up Orca
printCHECKPOINT "sourcing ORCA environment"
if ! [ -a $DarSetupFile ]; then
  printERROR "DAR setup file not found at $DarSetupFile"
  exit 1
else
  source $DarSetupFile
fi

#Stay we job lands - simply go down from here
#deprecated -- move to working area - create own dir
#cd $workDir
tempDir=`pwd`/$$
while [ -d $tempDir ]; do
  num=`echo "$$ + $x" | bc`
  tempDir=`pwd`/$num
  x=`echo "$x +1" | bc`
done

mkdir $tempDir
cd $tempDir

printCHECKPOINT "Now in working dir $tempDir"


#Copy files into workspace
printCHECKPOINT "Copying across input files"
filelist="$localFile"
for filename in $filelist; do
  if [ -a $filename ]; then
    cp $filename .
  else
    printERROR "Unable to find input file $filename"
    exit 1
  fi
done

#check pool catalog exists
export POOL_CATALOG="file:$tempDir/`basename $XMLFragFile`"
if [ -a ${tempDir}/`basename ${XMLFragFile}` ]; then
  echo "using POOL file catalog ${POOL_CATALOG}"
else
 printERROR "POOL catalog ${POOL_CATALOG} not found"
  exit 1
fi

printCHECKPOINT "Copying across input files and modifying local POOL file"
filelist="$InFile"
for filename in $filelist; do
  pfn=`FClistPFN -l $filename | cut -d# -f3`
  if [ -a $pfn ]; then
    cp $pfn .
    FCrenamePFN -p $pfn -n $tempDir/$filename
  else
    printERROR "Unable to find input file $filename at $pfn"
    exit 1
  fi
done

#Modify local .orcarc file
if [ -a .orcarc ]; then
  for run in $Run; do
      InputCollections="$InputCollections /System/$Owner/$Dataset/EvC_Run$run"
  done
  cat .orcarc | sed /InputCollections/d > out.txt
  cat out.txt | sed /PoolCatalog/d > out1.txt
  echo "InputCollections= ${InputCollections}" >> out1.txt
  mv out1.txt .orcarc
  rm out.txt
fi

#Run executable
printCHECKPOINT "Running user executable"

export LD_LIBRARY_PATH=${tempDir}:${LD_LIBRARY_PATH}
export PATH=${tempDir}:${PATH}

#check existence of use libraries
chmod +x ${userExec}
missingLibs=`ldd ${userExec} | grep -c "not found"`
if [ ${missingLibs} -ne 0 ]; then
  printERROR "Needed libraries not found"
  printERROR `ldd ${userExec}`
fi

printCHECKPOINT "Running user executable"


${userExec} ## Run Executable
orcaStatus=$?

#copy output files back to start directory
printCHECKPOINT "Copying output files"
data="${OutFile}"
for filename in ${data}; do
   if [  -a ${filename} ]; then
     printINFO "Copying ${filename} to output directory"
     cp ${filename} ${outDir}/${filename}.${Suffix}
   else
     printERROR "Output file not found ${filename}"
   fi
done

cd $STARTDIR
#remove all ORCA directories - not used for debugging etc
printINFO "Deleting working area..."
rm -rf ${tempDir}

printCHECKPOINT "ORCA finished with exit code $orcaStatus"
