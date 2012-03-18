#! /usr/bin/perl

# Preliminar version of submission to the grid with BOSS
# Doesn't allow to add files to inSandBox and outSandBox
# Doesn't allow to specify Input files from SE's
# Resource Broker is hardcoded

$len=@ARGV;

# scheduler local log file
$rbconfigstring="";
$dir=".";
$dg_log="dg.log";
open (LOG, ">>$dir/$dg_log") || die "Unable to write to local log file $dir/$dg_log";

if($len==4) {
    $executable = "jobExecutor";
    $jid = $ARGV[0];
    $logfile = mybasename($ARGV[1]);
    $host = $ARGV[2];
    $topwdir = $ARGV[3];
    
    $hoststring="";
    if ( $host ne "NULL" ) { 
	$hoststring="-r $host";
    }
    $rbconfigstring ="";

    print LOG "\n====>> New scheduler call number $jid\n";
    print LOG "$jid: Submitting $executable $jid... on host $host\n";
    print LOG "$jid: Redirecting stderr & stdout to log file $logfile\n";    
    
    # Query the DB and add files to sandboxes
    $q = "SELECT S_PATH,EXEC,STDIN,STDOUT,STDERR FROM JOB WHERE ID=$jid";
    &bossSQL($q);
# TEST
#    $sqlVal{S_PATH} = "/afs/cern.ch/user/g/grandic/boss-dg";
#    $sqlVal{EXEC} = "tmp/pippo.exec";
#    $sqlVal{STDIN} = "./pippo.in";
#    $sqlVal{STDOUT} = "tmp/pippo.out";
#    $sqlVal{STDERR} = "./pippo.err";
# END TEST
    $myexec   = absPath($sqlVal{S_PATH},$sqlVal{EXEC});
    $mystdin  = absPath($sqlVal{S_PATH},$sqlVal{STDIN});
    $mystdout = mybasename($sqlVal{STDOUT});
    $mystderr = mybasename($sqlVal{STDERR});
    
#    foreach $f (glob("Boss*")) {
#	print LOG " $f\n";
#    }

    $inSandBox  = "\"$ENV{BOSSDIR}/bin/$executable\",\"$myexec\"";
    $inSandBox  .= ",\"BossArchive_$jid.tgz\",\"$ENV{BOSSDIR}/bin/dbUpdator\"";
    $outSandBox = "\"$logfile\",\"BossJournal_$jid.txt\"";
    
    if(!($mystdin  =~ m#/dev/null#)) { 
	 $inSandBox.=",\"$mystdin\""; 
     }
     if(!($mystdout =~ m#/dev/null#)) { 
	 $outSandBox.=",\"$mystdout\""; 
     }
     if(!($mystderr =~ m#/dev/null#)) { 
	 $outSandBox.=",\"$mystderr\""; 
     }

#     $cladfile = "/tmp/BossClassAdFile_$jid";
# TEST
    $cladfile = "BossClassAdFile_$jid";
# END TEST
    $ppend2JDL = "";
    if ( -f $cladfile ) {
	%classad = &parseClassAd($cladfile);
        print LOG "Start dump ClassAd file:\n";
	foreach $ClAdKey ( keys %classad ) {
	    $ClAdVal = $classad{$ClAdKey};
	    print LOG "$ClAdKey = $ClAdVal;\n";
	    if ($ClAdKey eq "InputSandbox" || $ClAdKey eq "OutputSandbox" ) {
		$ClAdVal =~ s/\s*{(.*)}.*/$1/;
		@filelist = split(/,/,$ClAdVal);
		foreach $file (@filelist) {
		    $file =~ s/\s*"(.*)".*/$1/;
		    if     ($ClAdKey eq "InputSandbox" ) {
			$file = absPath($sqlVal{S_PATH},$file);
			if ( $inSandBox !~ m/$file/ ) {
			    $inSandBox.=",\"$file\"";
			} 
		    } elsif ($ClAdKey eq "OutputSandbox") {
			$file = mybasename($file);
			if ( $outSandBox !~ m/$file/ ) {
			    $outSandBox.=",\"$file\"";
			} 
		    }
		}

            } elsif ( $ClAdKey eq "RBconfig") {
                $ClAdVal =~ s/\s*{\s*\"\s*(.*)\s*\"\s*}\.*/$1/;
#               print "$ClAdKey : $ClAdVal \n";                
                chomp($ClAdVal);
                $rbconfig="-config $ClAdVal";
#               print "$rbconfigstring \n";

#Added by Stuart
	    } elsif ( $ClAdKey eq "RBconfigVO") {
                $ClAdVal =~ s/\s*{\s*\"\s*(.*)\s*\"\s*}\.*/$1/;
#               print "$ClAdKey : $ClAdVal \n";
                chomp($ClAdVal);
                $rbconfigVO="--config-vo $ClAdVal";
#END added 

	    } elsif ( $ClAdKey ne "" ) {
#		print "Appending $_ to jdl file\n";
#		if ( $ClAdVal =~ m/{.*}/ ) {
#		    $_ =~ s/\s*(.+)/$1/;
#		    $ppend2JDL.=$_;
#		}
#		else { 
		    $ppend2JDL.="$ClAdKey = $ClAdVal;\n";
#		    } 
	    }
        }
        print LOG "End dump ClassAd file\n";
    }

    # open a temporary file
    $tmpfile = `mktemp dg_XXXXXX` || die "error";
    chomp($tmpfile);

    open (JDL, ">$tmpfile") || die "error";
    # Executable submit with edg-job-submit
    print JDL ("Executable    = \"$executable\";\n");
    print JDL ("Arguments     = \"$jid $dir $topwdir localIO\";\n");
    # input,output,error files passed to executable
    # debug only
    print JDL ("StdOutput     = \"$logfile\";\n");
    print JDL ("StdError      = \"$logfile\";\n");
    print JDL ("InputSandbox  = {$inSandBox};\n");
    print JDL ("OutputSandbox = {$outSandBox};\n");
    print JDL $ppend2JDL;
    close(JDL);

    # submitting command
    $subcmd = "edg-job-submit -o ~/.bossEDGjobs $hoststring $rbconfig $rbconfigVO $tmpfile|";
    print LOG "subcmd = $subcmd\n";

#    exit;

    # open a pipe to read the stdout of edg-job-submit
    open (SUB, $subcmd);
    $id = "error";
    while ( <SUB> ) {
        print LOG;
	if ( $_ =~ m/https:(.+)/) {
	    print LOG "$jid: Scheduler ID = https:$1\n";
	    $id = "https:$1";
	}
#	if ( $_ =~ /\s+JOB SUBMIT OUTCOME\s*/ ) {
#	    $_ = <SUB>;
#	    if ( $_ =~ /\s*The job has been successfully submitted to the Resource Broker.\s*/ ) {
#		$_ = <SUB>;
#		$_ = <SUB>;
#		$_ = <SUB>;
#		if ( $_ =~ m/ - (.+)/ ) {		    
#		    print LOG "$jid: Scheduler ID = $1\n";
#		    $id = $1;
#		} else {	
#		    print LOG "$jid:ERROR: Unable to get the executable ID\n";  
#		    print "error";
#		}
#	    }	    
#	}
    }
    if ( $id eq "error" ) {
	print LOG "$jid:ERROR: Unable to submit the job\n";  
    }
    print $id;

    # close the file handles
    close(SUB);
    # delete temporary files
    #unlink "$tmpfile";
    #unlink "BossGeneralInfo_${jid}";
    #unlink "BossArchive_${jid}.tgz";
} else {
    print LOG "$jid:ERROR: Wrong number of arguments, use executable job_id log_file\n";
    print "error";
}
close(LOG);

sub bossSQL {
    $query = "boss SQL -query \"$_[0]\" |";
#    print LOG "bossSQL executing $query\n";
    open(SQL,$query); $fields=<SQL>; $head=<SQL>; $line=<SQL>; close(SQL);
    my @nn = split(",",$fields);
    my $N = $nn[0];
    $nn[0] = 0;
    my $offs = 1;
    foreach $i ( 1 .. $N ) {
	$offs += $nn[$i-1];
	my $key = substr($head,$offs,$nn[$i]); $key =~ s/\s+$//;
	my $val = substr($line,$offs,$nn[$i]); $val =~ s/\s+$//;
	$sqlVal{$key} = $val ;
#	print LOG "bossSQL returning $sqlVal{$key} = $val \n ";
    }
}

sub absPath {
#    print LOG "absPath called with arguments $_[0] $_[1] \n ";
    my $rf = $_[1];
    $rf =~ s/^.\///;
    if ( (!($rf =~ m#/dev/null#)) && (substr($rf ,0,1) ne "/")  ) {
	   $rf = $_[0]."/".$rf;
    }
#    print LOG "absPath returned $rf\n ";
    return $rf;
}

sub mybasename {
    my $rf = $_[0];
    $rf =~ s/^.\///;
    if ( !($rf =~ m#/dev/null#) ) {
	   $rf =~ s/.+\/// ;
    }
    return $rf;
}

sub parseClassAd {
    my $cladfile=$_[0];
    my $cladstring="";
    open (CLAD, $cladfile);
    while ( <CLAD> ) {
	$line = $_;
	chomp($line);
	$cladstring.=$line;
    }
    close(CLAD);
    if ( $cladstring =~ /.*\[(.+)\].*/ ) {
	$cladstring=$1;
    }
    my @attribs = split(/;/,$cladstring);
    foreach $attrib (@attribs) {
	if ( $attrib =~ /\s*(\w+)\s*=\s*(.+)/ ) {
	    $clad{$1}=$2;
	}
    }
    return %clad;
}
