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

#Start of Script
printCHECKPOINT "Wrapper script starting... on host `hostname` at `date` "
STEERFILE=$0.steer

#check steering file exists
if ! [ -a ${STEERFILE} ]; then 
    printERROR "Steering file ${STEERFILE} not found"
    exit 1
fi

#Set variables as listed in steering file 
printCHECKPOINT "Reading steering file"
extvble=`cat ${STEERFILE} | awk '{print $1}' | sort | uniq`
for vble in ${extvble}; do
  #comm=`grep ${vble} ${STEERFILE} | awk '{print $2}'`
  comm=`grep ${vble} ${STEERFILE} | awk -F"${vble} " '{print $2}'`
  vble=`echo $vble | sed  -r 's/\./_/g'` #will remove "." from hostname - not allowed in exported variable
  export $vble="$comm"
  echo "Setting variable from external steering file:" ${vble} ${comm[*]}
done

#copy original path and library path
STARTDIR=`pwd`
#echo "Startdir is: $STARTDIR"
STARTLD=$LD_LIBRARY_PATH

#Set up ORCA environment, move to a workspace
printCHECKPOINT "Creating ORCA environment"
cms_soft=$VO_CMS_SW_DIR
cd $cms_soft
cd $STARTDIR
if ! [ -d ${cms_soft} ]; then 
  printERROR "cms software area not found at ${cms_soft}"
  exit 1
fi

if ! [ -a ${cms_soft}/cmsset_default.sh ]; then 
  printERROR "cms setup shell script not found at ${cms_soft}/cmsset_default.sh"
  exit 1
fi

source ${cms_soft}/cmsset_default.sh
printCHECKPOINT "Setting up scram project ${orcaVers}..."
if [ -d ${orcaVers} ]; then
  printWARNING "ORCA already installed locally"
  else
  scram project ORCA ${orcaVers}
fi
cd $orcaVers/src

printCHECKPOINT "Setting ORCA environment..."
eval `scram runtime -sh`

workDir=`pwd`

#find VO of user and CE
export VO=`edg-brokerinfo getVirtualOrganization`
export CE=`edg-brokerinfo getCE | sed  -r 's/\./_/g' | awk -F":" '{print $1}'` #remove "." to be compatable with exported variable

#add current directory to path
export PATH="$PATH":`pwd`
printCHECKPOINT "Running job on node `hostname` in directory `pwd`"

#copy to ORCA dir untar and then copy to /src dir
cd ${STARTDIR}/$orcaVers
export filelist="${localInFile}"
printCHECKPOINT "Checking local input files from sandbox"
echo "List of input files are " ${filelist[*]}
for filename in ${filelist}; do
  if [ -a ${STARTDIR}/${filename} ]; then
    printINFO "Found file $filename"
    cp ${STARTDIR}/${filename} .
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
  printERROR "Missing input file ${STARTDIR}/${filename}"
  exit 1
  fi
done
cp * ${Orcarc} ${workDir}/.
cd ${workDir}

#build user executable if desired
#if [ $SCRAMBuildEXE = "true" ]; then
#  printCHECKPOINT "Building user code with SCRAM"
#  scram b
#  if [ "$?" -ne 0 ]; then
#    printERROR "Building of user code failed - see above - exiting"
#    exit 1
#  fi
#fi


#locate POOL catalog - download from remote server if neccesary
XMLcats=`env | grep $CE | cut -d"=" -f2`
for XMLcat in ${XMLcats}; do
  printINFO "Found POOL catalog ${XMLcat}"
  case ${XMLcat} in
    *http*)
      InputFileCatalogURL="${InputFileCatalogURL} ${XMLcat}";;
    *mysql*)
      InputFileCatalogURL="${InputFileCatalogURL} ${XMLcat}";;
    *:/*)
      XMLcat=`echo ${XMLcat} | awk -F"xmlcatalog_" '{print$2}'`
      newFile=`basename ${XMLcat}`
      rfcp ${XMLcat} ${newFile}
      InputFileCatalogURL="${InputFileCatalogURL} xmlcatalog_file:${workDir}/${newFile}";;
  esac
done      
      
for run in $Run; do
  InputCollections="$InputCollections /System/$Owner/$Dataset/EvC_Run$run"
done

#Edit .orcarc file
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

#Run ORCA executable
printCHECKPOINT "Running user executable"

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

printINFO "with command ${userExec} -c ${Orcarc} ${Arguments}"
${userExec} -c ${Orcarc} ${Arguments}         ## Run Executable
orcaStatus=$?

#copy RemoteDataOutputFile's to local SE and return LFN and GUID
printCHECKPOINT "Copying output files to local SE"
filelist="${RemoteDataOutputFile}"
for filename in ${filelist}; do
  LFN=${filename}${Suffix}
  if [ -a ${filename} ]; then
   exists=`edg-rm --vo $VO lr lfn:${LFN} | grep -c "Lfn does not exist"`
    if [ $exists -ne 0 ]; then
      GUID=`edg-replica-manager --vo $VO cr file://${workDir}/$filename --logical-file-name lfn:${LFN}`
      if [ $? -eq 0 ]; then
        printINFO "Copied $filename with LFN $filename$Suffix to GUID $GUID"
      else
        printERROR "Unable to copy file $filename to local SE"
        echo $GUID
      fi
    else
      printERROR "LFN ${LFN} already exists in catalogue"
    fi
  else
    printERROR "RemoteDataOutputFile $filename not found"
  fi
done

#copy other output files back to start directory
printCHECKPOINT "Copying output files"
data="${DataOutputFile} ${OutputSandboxFile} ${RemoteDataOutputFile}"
for filename in ${data}; do
   if [ -a ${filename} ]; then
     printINFO "Copying ${filename} to output directory"
     cp ${filename} ${STARTDIR}/${filename}${Suffix}
   else
     printERROR "Output file not found ${filename}"
   fi
done

#remove all ORCA directories
printINFO "Deleting working area..."
cd ${STARTDIR}
rm -rf ${STARTDIR}/${orcaVers}

printCHECKPOINT "Finished job at `date` with exit status $orcaStatus"
exit $orcaStatus
