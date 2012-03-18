#! /usr/bin/perl

$len=@ARGV;
if($len==5) {
    $executable = "jobExecutor";
    $jid = $ARGV[0];
    $logfile = $ARGV[1];
    $host = $ARGV[2];
    $topwdir = $ARGV[3];
    $copycomm = $ARGV[4];
    $dir=`pwd`;
    chomp $dir;

     # open a temporary file
     $tmpfile = "/tmp/jobExecutor-$jid.sh";
     open (CMD, ">$tmpfile") || die "error";
     # Script to submit executable with arguments
     print CMD ("#!/bin/sh\n");
     # on the execution host jobExecutor must be 
     # in the local $PATH or 
     # under the same path as in the submitting host:
     print CMD ("export PATH=\$PATH:$ENV{BOSSDIR}/bin\n");
     print CMD ("$executable $jid $dir $topwdir $copycomm\n");
     close(CMD);
     $subcmd = "qsub -o $logfile -j oe $tmpfile -W stagein=./BossArchive_$jid.tar@`hostname`:`pwd`/BossArchive_$jid.tar -W stageout=./BossOutArchive_$jid.tgz@`hostname`:`pwd`/BossOutArchive_$jid.tgz |";
    # open a pipe to read the stdout of bsub
    open (SUB, $subcmd);
    # find job id
    $_ = <SUB>;
    if ( $_ =~ /(\d+\.\S+)\S*/ ) {
         @tmp = split(/\./,$1);
         print "$tmp[0].$tmp[1]";
    } else {
        print "error";
    }

    # close the file handles
    close(SUB);
    # delete temporary file
    unlink "$tmpfile" || die "error";
} else {
    print "error";
}
