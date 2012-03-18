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
extvble=`cat ${STEERFILE} | awk '{print $1}' | uniq`
for vble in ${extvble}; do
  comm=`grep ${vble} ${STEERFILE} | awk '{print $2}'`
  export $vble="$comm"
  echo "Setting variable from external steering file:" ${vble} ${comm[*]}
done

#check for variables from  steering file, exit if not found
printCHECKPOINT "Checking variables set from ${STEERFILE}..."
echo ${orcaVers?"ERROR ! Variable not defined"} > /dev/null
echo ${Suffix?"ERROR ! Variable not defined"} > /dev/null
echo ${userExec?"ERROR ! Variable not defined"} > /dev/null
echo ${localInFile?"ERROR ! Variable not defined"} > /dev/null
echo ${XMLFragFile?"ERROR ! Variable  not defined"} > /dev/null

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

#find VO of user
export VO=`edg-brokerinfo getVirtualOrganization`

#add current directory to path
export PATH="$PATH":`pwd`
printCHECKPOINT "Running job on node `hostname` in directory `pwd`"

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
        tar xvf $filename;;
    esac
  else 
  printERROR "Missing input file ${STARTDIR}/${filename}"
  exit 1
  fi
done

#build user executable if desired
if [ $SCRAMBuildEXE = "true" ]; then
  printCHECKPOINT "Building user code with SCRAM"
  scram b
  if [ "$?" -ne 0 ]; then
    printERROR "Building of user code failed - see above - exitting"
    exit 1
  fi
fi


#check pool catalog exists
export POOL_CATALOG="xmlcatalog_file:${workDir}/${XMLFragFile}"
if [ -a ${workDir}/${XMLFragFile} ]; then
  echo "using POOL file catalog ${POOL_CATALOG}"
else
 printERROR "POOL catalog ${POOL_CATALOG} not found"
  exit 1
fi


#copy remote files into workspace
#first find lfn from steer file
#then rename pool replica to point to local copy
files="${RemoteDataInputFile}"
for lfn in ${files}; do
  printCHECKPOINT "getting remote file ${lfn} ..."
  surl=`edg-rm --vo ${VO} getBestFile lfn:${lfn}`
  printINFO "  from ${surl}"
  SE=`echo ${surl} | awk -F/ '{print $3;}'`
  PROTOCOLS=`edg-brokerinfo -v getSEProtocols ${SE}` # |  sed -r 's/gsiftp//g'` # used for debugging removes given protocol
  case "$PROTOCOLS" in
       *rfio*)
         printINFO "  via rfio"
         rfioname=`echo $surl | sed -e "s?sfn://$SE?$SE:?"`
         fname=rfio:$rfioname;;
       *gsiftp*)
         printINFO "  via gridftp"
         fname=${workDir}/${lfn}
         edg-rm --vo cms cp $surl file://$fname;;
  esac
  if [ $? -eq 0 ]; then 
    printINFO "  file copied to ${fname}"
  else
    printERROR "Unable to copy ${lfn} to ${fname}"
  fi
  PFNs=`FClistPFN -l ${lfn}`
  printINFO "  updating POOL catalog for ${lfn}" 
  for pfn in ${PFNs}; do
    FCrenamePFN -p ${pfn} -n ${fname}
  done 
done

#Get metadata from RefDB using Julia's script and add POOL info to catalog
#Currently cannot handle unatached metadata
printCHECKPOINT "Obtaining MetaData from RefDB and changing POOL catalog"
filelist=$MetaDataFile
for file in $filelist; do
  printINFO "Getting meta file $file"
  wget "http://cmsdoc.cern.ch/cms/production/publish/PRS/DST/$Owner/$Dataset/$file"
done

#rename paths to local
sed -r 's/\${VirginMETAPath}/./g' ${workDir}/${XMLFragFile}  > ${workDir}/${XMLFragFile}.new
mv ${workDir}/${XMLFragFile}.new ${workDir}/${XMLFragFile}

for run in $Run; do
  InputCollections="$InputCollections /System/$Owner/$Dataset/EvC_Run$run"
done  


#Edit .orcarc file
printCHECKPOINT "Modifying users .orcarc file"
cat .orcarc | sed /InputCollection/d > out.txt
cat out.txt | sed /PoolCatalog/d > out1.txt
mv out1.txt .orcarc
rm out.txt
echo "InputCollections = ${InputCollections}" >> .orcarc
echo "TextColor=False" >> .orcarc

#redo scram runtime
eval `scram runtime -sh`

#Run ORCA executable
printCHECKPOINT "Running user executable"

export LD_LIBRARY_PATH=${workDir}:${LD_LIBRARY_PATH}
export PATH=${workDir}:${PATH}

#check existence of user libraries
userExecPath=`which ${userExec}`
chmod +x $userExecPath #if allowed
missingLibs=`ldd $userExecPath | grep -c "not found"`
if [ ${missingLibs} -ne 0 ]; then
  printERROR "Needed libraries not found"
  ldd $userExecPath
fi

${userExec} ## Run Executable
orcaStatus=$?

#copy RemoteDataOutputFile's to local SE and return LFN and GUID
printCHECKPOINT "Copying output files to local SE"
filelist="$RemoteDataOutputFile"
for filename in $filelist; do
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
data="${DataOutputFile} ${OutputSandboxFile}"
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

printCHECKPOINT "Finished jod at `date` with exit status $orcaStatus"
exit $orcaStatus
