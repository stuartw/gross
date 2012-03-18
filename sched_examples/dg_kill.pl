#! /usr/bin/perl
$status="error";
$len=@ARGV;
$dir=".";
$dg_log="dg.log";
open (LOG, ">>$dir/$dg_log") || die "Unable to write to local log file $dir/$dg_log";
if($len==1) {
    $sid=$ARGV[0];
    $killcmd = "edg-job-cancel -noint $sid |";
    print LOG "\n====>> Kill request for job $sid\n";
    open (KILL, $killcmd);
    while ( <KILL> ) {
	print LOG;
	if ( $_ =~ m/ The cancellation request has been successfully submitted/ ) {
	    $status = "killed";
	}
    }
}
print LOG "Result of kill request is: $status\n";
print "$status\n";


