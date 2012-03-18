#!/bin/bash
#set -x

#Functions for producing BOSS viewable monitoring info
printERROR() {
  echo "orca_grid_wrapper:ERROR: " $@>&1
}

printWARNING() {
  echo "orca_grid_wrapper:WARNING: " $@>&1
}

printCHECKPOINT() {
  echo "orca_grid_wrapper:CHECKPOINT: " $@>&1
}

printINFO() {
  echo "orca_grid_wrapper:INFO: " $@>&1
}


checkVariable() {
  variable=$1
  defined=`env | grep -c ${variable}`
  if [ ${defined} -eq 0 ]; then
    printERROR "${variable} not defined"
  fi
}


#Start of Script
printCHECKPOINT "Wrapper script starting..."


#will be done automatically on real worker node
#export EDG_WL_RB_BROKERINFO=`pwd`/.BrokerInfo
# remove when used for real

#if [ $# != 1 ]; then
  #printERROR "Need steering file name"
  #exit 1
#else
   STEERFILE=$0.steer
#fi



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
if ! [ -d ${cms_soft} ]; then 
  printERROR "cms software area not found at ${cms_soft}"
  exit 1
fi

if ! [ -a ${cms_soft}/setup/bashrc ]; then 
  printERROR "cms setup shell script not found at ${cms_soft}/cmsset_default.sh"
  exit 1
fi

echo "Sourcing shrc..."
source ${cms_soft}/setup/bashrc
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
cd ${workDir}

#*****SHOULD BE DONE BY EVAL `SCRAM RUNTIME -SH` BUT NOT YET
#LD=`echo $LD_LIBRARY_PATH | sed -e 's/cms\//stage\/exp_software\/cms\//g'`
#SP=`echo $SEAL_PLUGINS | sed -e 's/cms\//stage\/exp_software\/cms\//g'`
#PA=`echo $PATH | sed -e 's/cms\//stage\/exp_software\/cms\//g'`
#export LD_LIBRARY_PATH=${LD}
#export SEAL_PLUGINS=${SP}
#export PATH=${PA}

#find VO of user
export VO=`edg-brokerinfo getVirtualOrganization`

#add current directory to path
export PATH="$PATH":`pwd`
#echo "Running job in " `pwd`

#copy files into workspace
#set filelist="${DATATAR} ${LIBTAR} ${EXECFILE} ${RCFILE}"
export filelist="${localInFile}"
printCHECKPOINT "Checking local input files from sandbox"
echo "List of input files are " ${filelist[*]}
for filename in ${filelist}; do
    if [ -a ${STARTDIR}/${filename} ]; then
    printINFO "Found file $filename"
    cp ${STARTDIR}/${filename} .
  else 
  printERROR "Missing input file ${STARTDIR}/${filename}"
  exit 1
  fi
done

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
files="${RemoteDataInputFile} ${MetaDataFile}"
for lfn in ${files}; do
  printCHECKPOINT "getting remote file ${lfn} ..."
  #surl=`edg-brokerinfo getLFN2SFN lfn:${lfn}`
  surl=`edg-rm --vo ${VO} getBestFile lfn:${lfn}`
  printINFO "  from ${surl}"
  SE=`echo ${surl} | awk -F/ '{print $3;}'`
  PROTOCOLS=`edg-brokerinfo getSEProtocols ${SE}` # |  sed -r 's/gsiftp//g'`
  case "$PROTOCOLS" in
       *file*)
         printINFO "  via file method"
	 turl=`edg-replica-manager --vo ${VO} --insecure getTurl ${surl} file`
         fname=`echo ${turl} | sed -r 's/file:\/\///g;;'`;;
       *rfio*)
         printINFO "  via rfio"
         #turl=`edg-replica-manager --vo ${VO} --insecure getTurl ${surl} rfio`
         #rfioname=`echo ${turl} | sed -r 's/rfio:\/\///g' | sed -r 's/\//:\//'`
         rfioname=`echo $surl | sed -e "s?sfn://$SE?$SE:?"`
         if [ `echo $lfn | grep -c "META"` -ne 0 ]; then
           METAFILE=$rfioname
	   META_LFN=$lfn
         fi
         fname=rfio:$rfioname;;
         #fname=${workDir}/${lfn}
         #rfcp ${rfcpname} ${workDir}/${lfn};;
       *gsiftp*)
         printINFO "  via gridftp"
         turl=`edg-replica-manager --vo ${VO} --insecure getTurl ${surl} gridftp`
         fname=${workDir}/${lfn}
         globus-url-copy ${turl} file:${fname};;
  esac
  if [ $? -eq 0 ]; then 
    printINFO "  file copied to ${fname}"
  else
    printERROR "Unable to copy ${lfn} to ${fname}"
  fi
  PFNs=`FClistPFN -l ${lfn}` # | awk -F# '{ print $3 ;}'
  printINFO "  updating POOL catalog for ${lfn}" 
  for pfn in ${PFNs}; do
    FCrenamePFN -p ${pfn} -n ${fname}
  done 
done

#Copy across MetaData
# "Now getting MetaData"
#storage_element=diskserv-cms-2.cr.cnaf.infn.it
#RLS=edgcatalog_http://rlscms.cern.ch:7777/cms/v2.2/edg-local-replica-catalog/services/edg-local-replica-catalog
#echo "Searching RLS"
#META_PFN=`FClistPFN -q "DataType='ZippedMETA' and dataset='${DATASET}' and owner='${OWNER}'" -u $RLS | grep $storage_element | tail -1`
#META_LFN=`basename $META_PFN`
#TURL_Zipped=`echo $META_PFN | sed -e "s?sfn?gsiftp?"`
#echo "Getting MetaData at ${META_PFN}"
#globus-url-copy $TURL_Zipped file:${workDir}/$META_LFN
#Unzip Meta Data


printCHECKPOINT "Unzipping MetaData and fixing POOL catalog"

#METAFILE=`FClistLFN -q "DataType='ZippedMETA' and dataset='${DATASET}' and owner='${OWNER}'"`
#METAFILE=$rfioname 
rfcp $METAFILE ${workDir}/${META_LFN}
#META_LFN=`ls *.zip*`
/usr/java/j2sdk1.4.1_01/bin/jar xvf ${META_LFN}
#Change POOL catalog
META_CAT=`ls POOL_Catalogue_PCP*.xml`
META_CAT=file:`pwd`/$META_CAT
printINFO "Now publishing META files to ${POOL_CATALOG}"
for owner in `ls * | grep "\.META\." | cut -d. -f3 | sort | uniq`; do
  FCpublish -q "DataType='META' and owner='$owner'" -u ${META_CAT} -d ${POOL_CATALOG}
done

printCHECKPOINT "Chnaging POOL catalog to point to local files"
for old in `FClistPFN -u ${POOL_CATALOG} | grep META` ; do
  #if [ ${old} ]; then
   file=`basename ${old}`
   FCrenamePFN -p ${old} -n ${workDir}/${file} -u ${POOL_CATALOG}
  #fi
done

#Do COBRA findColls on EVD0 file - should only be one as one run per split job
printINFO "Using cobra findColls to find collection strings"
#EVD_FILE=`ls *EVD0*`
for x in $RemoteDataInputFile; do
 if [ `echo $x | grep -c EVD0` -ge 1 ]; then
   EVD_FILE=`FClistPFN -l $x | sort | uniq`
#    #for lfn in $lfns; do 
   findColls ${EVD_FILE} --PrintOID --InputFileCatalogURL=${POOL_CATALOG} > coll.out
   InputCollections=`cat coll.out | grep "EvC_Run" | cut -d ' ' -f2`
    # then add to .orcarc file
   cat .orcarc | sed /InputCollection/d > out.txt
   cat out.txt | sed /PoolCatalogFile/d > out1.txt
   mv out1.txt .orcarc
   echo "InputCollections = @{${InputCollections}}@" >> .orcarc
 fi
done

#edit .orcarc file to point to local POOL Catalog
#if [ `grep -c PoolCatalogFile ${workDir}/.orcarc` -ge 1 ]; then
#  awk '// { if ( $1 == "PoolCatalogFile" ) {print "PoolCatalogFile = " POOL;} else print;}' POOL="$POOL_CATALOG" .orcarc > .orca.tmp
#else
#  echo "PoolCatalogFile = @{"${POOL_CATALOG}"}@" > out.txt
#  cat out.txt .orcarc > .orca.tmp
#  rm out.txt
#fi
#cp .orca.tmp .orcarc
#rm .orca.tmp
#echo $POOL_CATALOG

#redo scram runtime
eval `scram runtime -sh`

#Run ORCA executable
printCHECKPOINT "Running user executable"

export LD_LIBRARY_PATH=${workDir}:${LD_LIBRARY_PATH}
export PATH=${workDid}:${PATH}

#check existence of use libraries
chmod +x ${userExec}
missingLibs=`ldd ./${userExec} | grep -c "not found"`
if [ ${missingLibs} -ne 0 ]; then
  printERROR "Needed libraries not found"
  printERROR `ldd ./${userExec}`
fi

./${userExec} ## Run Executable
orcaStatus=$?

#copy output files back to start directory
printCHECKPOINT "Copying output files"
data="${DataOutputFile} ${OutputSandboxFile}"
for filename in ${data}; do
   if [  -a ${filename} ]; then
     printINFO "Copying ${filename} to output directory"
     cp ${filename} ${STARTDIR}/${filename}.${Suffix}
   else
     printERROR "Output file not found ${filename}"
   fi
done

#remove all ORCA directories
printINFO "Deleting working area..."
#rm -rf ${STARTDIR}/${orcaVers}

printCHECKPOINT "Finished with exit status $orcaStatus"
