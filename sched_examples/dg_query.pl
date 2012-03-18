#! /usr/bin/perl

# return couples sid state for all jobs idle and running
#$dir=".";
#$dg_log="dg.log";
#open (LOG, ">>$dir/$dg_log") || die "Unable to write to local log file $dir/$dg_log";

$command = "edg-job-status -i ~/.bossEDGjobs -noint |";
open (QUERY , $command );

while ( <QUERY> ) {
    if ( $_ =~ /Status info for the Job :\s+(.*)\s*/ ) {
	$sid = $1;
	$_ = <QUERY>;
	if ( $_ =~ /Current Status:\s+(\w+)\s*/ ) {
	    $stat{$sid} = $1;
	}
    }
}
#print LOG "Dump current situation:\n";
foreach $sid ( keys %stat ) {
#    print LOG "$sid $stat{$sid} ";
    if      ( $stat{$sid} eq "Running" ||
	      $stat{$sid} eq "Checkpointed" ) {
	print "$sid R\n";
#	print LOG "R\n";
    } elsif ( $stat{$sid} eq "Scheduled" || 
	      $stat{$sid} eq "Ready" ||
	      $stat{$sid} eq "Waiting" ||
	      $stat{$sid} eq "Submitted" ||
	      $stat{$sid} eq "Undefined" ) {
	print "$sid I\n";
    } elsif ( $stat{$sid} eq "Cancelled" ) {
	print "$sid MK\n";
    } elsif ( $stat{$sid} eq "Done" ) {
	print "$sid ME\n";
#	print LOG "I\n";
#    } else {
#	print LOG "?\n";
    }
}
#print LOG "End dump durrent situation:\n";
