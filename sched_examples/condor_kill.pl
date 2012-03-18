#! /usr/bin/perl
$status="error";
$len=@ARGV;
if($len==1) {
    $sid=$ARGV[0];
    $tmpfile = `mktemp condor_XXXXXX` || die "error";
    chomp($tmpfile);
    system "condor_rm $sid >& $tmpfile";
    open (K,$tmpfile);
    while (<K>) {
	if($_ =~ /Cluster\s+\d+\s+has been marked for removal../) {
   	  $status = "killed";
	}
    }
    close(K);
    unlink($tmpfile);
}
print "$status\n";

