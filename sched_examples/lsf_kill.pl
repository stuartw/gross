#! /usr/bin/perl
$len=@ARGV;
if($len==1) {
    $sid=$ARGV[0];

    $killcmd = "bkill $sid |";

    # open a pipe to read the stdout of bsub
    open (KILL, $killcmd);
    # find job id
    $_ = <KILL>;
    if ( $_ =~ /Job \<(\d+)\>\s+is being terminated\s+/ ) {
        print "killed";
    } else {
        print "error";
    }
    close(KILL);
}
