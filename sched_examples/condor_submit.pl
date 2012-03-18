#! /usr/bin/perl

$len=@ARGV;

# scheduler local log file
$dir=`pwd`;
chomp $dir;
$condor_log="condor.log";
open (LOG, ">>$dir/$condor_log") || die "Unable to write to local log file $dir/$condor_log";

if($len==5) {
    $executable = "$ENV{BOSSDIR}/bin/jobExecutor";
    $jid = $ARGV[0];
    $logfile = $ARGV[1];
    $host = $ARGV[2];
    $topwdir = $ARGV[3];
    $copycomm = $ARGV[4];

    # open a temporary file
    $tmpfile = `mktemp condor_XXXXXX` || die "error";
    chomp($tmpfile);

    print LOG "\n====>> New scheduler call number $jid\n";
    print LOG "$jid: Submitting $executable $jid... on host $host\n";
    print LOG "$jid: Redirecting stderr & stdout to log file $logfile\n";    

    open (CMD, ">$tmpfile") || die "error";
    # Executable submit to Condor
    print CMD ("Executable  = $executable\n");
    print CMD ("Arguments   = $jid $dir $topwdir $copycomm\n");
    # Type of Universe (i.e. Standard, Vanilla, PVM, MPI, Globus)
    print CMD ("Universe    = vanilla\n");
    # input,output,error files passed to executable
    # debug only
    print CMD ("output      = $logfile\n");
    print CMD ("error       = $logfile\n");
    # Condor log file
    print CMD ("log         = $dir/$condor_log\n");
    # Transfer files
    print CMD ("should_transfer_files = YES\n");
    print CMD ("when_to_transfer_output = ON_EXIT\n");
    print CMD ("transfer_input_files = BossArchive_$jid.tgz\n");
    print CMD ("transfer_output_files = BossOutArchive_$jid.tgz\n");
    # A string to help finding boss jobs in condor
    print CMD ("+BossJob = $jid\n");
    $domain = ` dnsdomainname`;
    chomp($domain);
    if ( $host ne "NULL" ) { 
	if ( $host =~ /\s+\.\s+/ ) {
	    print CMD ("Requirements = Machine == \"$host\"\n");
	} else {
	    print CMD ("Requirements = Machine == \"$host.$domain\"\n");
	}
    }
    # if getenv is set true the executable inherit all of the user's current 
    # shell environment variables at the time of job submission.
    # on the execution host jobExecutor must be 
    # in the local $PATH or 
    # under the same path as in the submitting host:
    print CMD ("Getenv      = True\n");
    print CMD ("Queue 1\n");  
    close(CMD);

    # submitting command
    $subcmd = "condor_submit $tmpfile |";
    # print "$subcmd\n";

    # open a pipe to read the stdout of condor_submit
    open (SUB, $subcmd);
    # skip the first two line

    $_ = <SUB>;
    $_ = <SUB>;
    # find cluster id
    $_ = <SUB>;
    if ( $_ =~ /\d+\s+.+\s+submitted to cluster\s+(\d+)/ ) {
        print LOG "$jid: Scheduler ID = $1\n";
	print $1;
    } else {
	print LOG "$jid:ERROR: Unable to submit the executable\n";  
	print "error";
    }
    # close the file handles
    close(SUB);
    # delete temporary file
    unlink "$tmpfile" || die "error";
    print LOG "tmpfile is $tmpfile\n";
} else {
    print LOG "$jid:ERROR: Wrong number of arguments, use executable job_id log_file\n";
    print "error";
}
close(LOG);




