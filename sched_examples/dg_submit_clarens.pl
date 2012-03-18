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

if($len==5) {
    $executable = "jobExecutor";
    $jid = $ARGV[0];
    $logfile = mybasename($ARGV[1]);
    $host = $ARGV[2];
    $topwdir = $ARGV[3];
    $copycomm = $ARGV[4];
    
    $hoststring="";
    if ( $host ne "NULL" ) { 
	$hoststring="-r $host";
    }
    $rbconfigstring ="";

    print LOG "\n====>> New scheduler call number $jid\n";
    print LOG "$jid: Submitting $executable $jid... on host $host\n";
    print LOG "$jid: Redirecting stderr & stdout to log file $logfile\n";    
    
    

    $inSandBox  = "\"$ENV{BOSSDIR}/bin/$executable\"";
    $inSandBox  .= ",\"BossArchive_$jid.tgz\"";
    $outSandBox = "\"$logfile\",\"BossOutArchive_$jid.tgz\"";
    print LOG "CHECK output sandbox is $outSandBox\n";
    print LOG "gets here\n";

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

           if ( $ClAdKey eq "RBconfig") {
                $ClAdVal =~ s/\s*{\s*\"\s*(.*)\s*\"\s*}\.*/$1/;
               print "$ClAdKey : $ClAdVal \n";                
                chomp($ClAdVal);
                $rbconfigstring="-config $ClAdVal";
               print "$rbconfigstring \n";
	    } elsif ( $ClAdKey eq "RBconfigVO") {
                $ClAdVal =~ s/\s*{\s*\"\s*(.*)\s*\"\s*}\.*/$1/;
               print "$ClAdKey : $ClAdVal \n";
                chomp($ClAdVal);
                $rbconfigVO="--config-vo $ClAdVal";
	    } elsif ( $ClAdKey ne "" ) {
		print "Appending $_ to jdl file\n";
		if ( $ClAdVal =~ m/{.*}/ ) {
		    $_ =~ s/\s*(.+)/$1/;
		    $ppend2JDL.=$_;
		}
		else { 
		    $ppend2JDL.="$ClAdKey = $ClAdVal;\n";
		    } 
	    }
        }
        print LOG "End dump ClassAd file\n";
    }

    print LOG "Made it to here\n";
    # open a temporary file
    $tmpfile = `mktemp dg_XXXXXX` || die "error";
    chomp($tmpfile);

    open (JDL, ">$tmpfile") || die "error";
    # Executable submit with edg-job-submit
    print JDL ("Executable    = \"$executable\";\n");
    print JDL ("Arguments     = \"$jid $dir $topwdir $copycomm\";\n");
    # input,output,error files passed to executable
    # debug only
    print JDL ("StdOutput     = \"$logfile\";\n");
    print JDL ("StdError      = \"$logfile\";\n");
    print JDL ("InputSandbox  = {$inSandBox};\n");
    print JDL ("OutputSandbox = {$outSandBox};\n");
    print JDL $ppend2JDL;
    close(JDL);

    # submitting command
    $subcmd = "edg-job-submit -o /tmp/.bossEDGjobs $hoststring $rbconfigstring $rbconfigVO $tmpfile|";
    print LOG "subcmd = $subcmd\n";

#    exit;

    # open a pipe to read the stdout of edg-job-submit
    system("which edg-job-submit >/tmp/dg_submit.log 2>&1"); 
    system("edg-job-submit -o /tmp/.bossEDGjobs $hoststring $rbconfigstring $rbconfigVO $tmpfile > /tmp/out.log 2>&1" );
    open (SUB, $subcmd);
    $id = "error";
    while ( <SUB> ) {
        print STDERR $_;
#        print LOG;
	if ( $_ =~ m/https:(.+)/) {
	    print LOG "$jid: Scheduler ID = https:$1\n";
	    $id = "https:$1";
	}
    }
    if ( $id eq "error" ) {
	print LOG "$jid:ERROR: Unable to submit the job\n";  
    }
    print $id;

    # close the file handles
    close(SUB);
    # delete temporary files
    unlink "$tmpfile";
    unlink "BossArchive_${jid}.tgz";
} else {
    print LOG "$jid:ERROR: Wrong number of arguments, use executable job_id log_file\n";
    print "error";
}
close(LOG);


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
