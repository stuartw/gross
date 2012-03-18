#! /usr/bin/perl
$len=@ARGV;
if($len==1) {
    $sid=$ARGV[0];
    $ret=system "qdel $sid >& /dev/null";
    if ( $ret == "0" ) {
	print "killed";
    } else {
        print "error";
    }
}
