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
    print LOG "Killing with command $killcmd\n";
    print LOG "*** Start dump of kill request:\n";
    open (KILL, $killcmd);
    while ( <KILL> ) {
	print LOG;
	if ( $_ =~ /\s+edg-job-cancel Success\s*/ ) {
	    $status = "killed";
	}
    }
    print LOG "*** End dump of kill request:\n";
}
print LOG "Result of kill request is: $status\n";
print "$status\n";
