#! /usr/bin/perl
$|=1;
$len=@ARGV;
# scheduler local log file
$dir=`pwd`;
chomp $dir;
$fork_log="fork.log";
open (LOG, ">>$dir/$fork_log") || die "Unable to write to local log file $dir/$fork_log";

if($len==4) {
    $executable = "jobExecutor";
    $jid = $ARGV[0];
    $logfile = $ARGV[1];
    $host = $ARGV[2];
    $topwdir = $ARGV[3];
    print LOG "\n====>> New scheduler call number $jid\n";
    print LOG "$jid: Submitting $executable $jid...\n";
    print LOG "$jid: Redirecting stderr & stdout to log file $logfile\n";
    if ( $host ne "NULL" ) { 
	print LOG "Submission to $host request is ignored:\n";
	print LOG "fork scheduler only allows submission to current host\n";
    }
    $cladfile = "/tmp/BossClassAdFile_$jid";
    if ( -f $cladfile ) {
        print LOG "Start dump ClassAd file:\n";
	open (CLAD, $cladfile);
	while ( <CLAD> ) {
	   print LOG $_;
        }
        print LOG "End dump ClassAd file\n";
    }
    if ( !defined($pid=fork()) ) {
	print LOG "$jid:ERROR: Unable to fork the executable\n";  
	print "0::0::0\n";
    }
    elsif ($pid == 0) {
	# child
	sleep 1;
	if ( exec("$executable $jid `pwd` $topwdir 1>>$logfile 2>&1" ) != 0 ) {
	    print LOG "$jid:ERROR: exec($executable) failed";
	}
	exit(0);
    } else {
        # father
	print LOG "$jid: $executable $jid submitted with pid $pid\n";
	print LOG "$jid: Scheduler ID = ${executable}::${jid}::${pid}\n";
	close(LOG);
        print "${executable}::${jid}::${pid}\n";
    } 
} else {
    print LOG "$jid:ERROR: Wrong number of arguments, use executable job_id log_file\n";
    close(LOG);   
    print "0::0::0\n";
}

