#! /usr/bin/perl

$len=@ARGV;
if($len==4) {
    # on the execution host jobExecutor must be 
    # under the same path as in the submitting host:
    $executable = "$ENV{BOSSDIR}/bin/jobExecutor";
    $jid = $ARGV[0];
    $logfile = $ARGV[1];
    $host = $ARGV[2];
    $topwdir = $ARGV[3];
    $dir=`pwd`;
    chomp $dir;

    $hoststring="";
    $domain = ` dnsdomainname`;
    chomp($domain);
    if ( $host ne "NULL" ) { 
	if ( $host =~ /\s+\.\s+/ ) {
            $hoststring="-m $host";
	} else {
            $hoststring="-m $host.$domain";
	}
    }

    $subcmd = "bsub -o $logfile $hoststring -e $logfile $executable $jid $dir $topwdir |";

    # open a pipe to read the stdout of bsub
    open (SUB, $subcmd);
    # find job id
    $_ = <SUB>;
    if ( $_ =~ /Job \<(\d+)\>\s+is submitted\s+/ ) {
        print $1;
    } else {
        print "error";
    }

    # close the file handles
    close(SUB);
} else {
    print "error";
}
