#!/bin/bash
set -x

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
printCHECKPOINT "Wrapper script starting... on host `hostname` at `date` "

outDir=$1
STEERFILE="$0.steer"

#Check steering file exists
if ! [ -a ${STEERFILE} ]; then
    printERROR "Steering file ${STEERFILE} not found"
    exit 1
fi

#Set variables as listed in steering file
printCHECKPOINT "Reading steering file"
extvble=`cat ${STEERFILE} | awk '{print $1}' | sort | uniq`
for vble in ${extvble}; do
  comm=`grep ${vble} ${STEERFILE} | awk -F"${vble} " '{print $2}'`
  vble=`echo $vble | sed  -r 's/\./_/g'` #will remove "." from hostname - not allowed in exported variable
  export $vble="$comm"
  echo "Setting variable from external steering file:" ${vble} ${comm[*]}
done

STARTDIR=`pwd`
STARTLD=$LD_LIBRARY_PATH


#Stay where job lands - simply go down from here
#create unique directory - cycle till find one that does not exist
tempDir=`pwd`/$$
x=
while [ -d $tempDir ]; do
  x=`echo "$x +1" | bc`
  num=`echo "$$ + $x" | bc`
  tempDir=`pwd`/$num
done

mkdir $tempDir
cd $tempDir

printCHECKPOINT "Now in working dir $tempDir"

printCHECKPOINT "Setting up scram project ${orcaVers}..."
if [ -a ${Scram} ]; then
  ${Scram} project ORCA ${orcaVers}
else
  printERROR "SCRAM executable not found/not defined at ${Scram}"
  exit 1
fi

#Copy files into workspace
cd ${orcaVers}
workDir="${tempDir}/${orcaVers}/src"
printCHECKPOINT "Copying across input files"
filelist="$localFile"
for filename in $filelist; do
  if [ -a "${STARTDIR}/$filename" ]; then
    cp "${STARTDIR}/${filename}" .
    # Untar file if neccesary
    case "$filename" in
      *.tar.gz)
        printINFO "Untarring file $filename"
        tar xvzf $filename;;
      *.tgz)
        printINFO "Untarring file $filename"
        tar xvzf $filename;;
      *.tar)
        printINFO "Untarring file $filename"
        tar xf $filename;;
    esac
  else
    printERROR "Unable to find input file $filename"
    exit 1
  fi
done
cp * ${Orcarc} ${workDir}/.
cd ${workDir}


#locate POOL catalog - download from remote server if neccesary
export DOMAIN=`dnsdomainname | sed  -r 's/\./_/g' | awk -F":" '{print $1}'` #remove "." to be compatable with exported variable
XMLcats=`env | grep $DOMAIN | cut -d"=" -f2`
for XMLcat in ${XMLcats}; do
  printINFO "Found POOL catalog ${XMLcat}"
  case ${XMLcat} in
    *http*)
      InputFileCatalogURL="${InputFileCatalogURL} ${XMLcat}";;
    *mysql*)
      InputFileCatalogURL="${InputFileCatalogURL} ${XMLcat}";;
    *:\*)
      ${XMLcat}=`echo ${XMLcat} | awk -F"xmlcatalog_" '{print$2}'`
      newFile=`uuidgen`.xml
      rfcp ${XMLcat} ${newFile}
      InputFileCatalogURL="${InputFileCatalogURL} xmlcatalog_file:${workDir}/${newFile}";;
  esac
done

for run in $Run; do
  InputCollections="$InputCollections /System/$Owner/$Dataset/EvC_Run$run"
done


#Modify local .orcarc file
printCHECKPOINT "Modifying users ${Orcarc} file"
if [ -a ${Orcarc} ]; then
  cat ${Orcarc} | sed /InputCollection/d > out.txt
  cat out.txt | sed /PoolCatalog/d > out1.txt
  cat out1.txt | sed /InputFileCatalog/d > out2.txt
  mv out2.txt ${Orcarc}
  rm out.txt out1.txt
  echo "InputCollections = ${InputCollections}" >> ${Orcarc}
  echo "InputFileCatalogURL = @{ ${InputFileCatalogURL} }@" >> ${Orcarc}
  echo "TextColor=False" >> ${Orcarc}
else 
  printERROR "unable to locate orcarc file at ${Orcarc}"
  exit 1
fi

printINFO "Orcarc contents are"
cat ${Orcarc}
printINFO "End Orcarc contents"

#do scram runtime
eval `scram runtime -sh`

#Run executable
export SCRAM_ARCH=`scram arch`
export LD_LIBRARY_PATH=${LOCALRT}/lib/${SCRAM_ARCH}:${LD_LIBRARY_PATH}
export PATH=${LOCALRT}/bin/${SCRAM_ARCH}:${PATH}

#check existence of user libraries
userExecPath=`which ${userExec}`
chmod +x $userExecPath #if allowed
missingLibs=`ldd $userExecPath | grep -c "not found"`
if [ ${missingLibs} -ne 0 ]; then
  printERROR "Needed libraries not found"
  ldd $userExecPath
  exit -1
fi

printCHECKPOINT "with command ${userExec} -c ${Orcarc} ${Arguments}"
${userExec} -c ${Orcarc} ${Arguments}         ## Run Executable
orcaStatus=$?


#copy output files back to start directory
printCHECKPOINT "Copying output files"
data="${OutFile}"
for filename in ${data}; do
   if [ -a ${filename} ]; then
     printINFO "Copying ${filename} to output directory"
     cp ${filename} ${STARTDIR}/${filename}${Suffix}
   else
     printERROR "Output file not found ${filename}"
   fi
done

cd $STARTDIR
#remove all ORCA directories
printINFO "Deleting working area..."
rm -rf ${tempDir}

printCHECKPOINT "Finished job at `date` with exit status $orcaStatus"
exit ${orcaStatus}
