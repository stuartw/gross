#! /usr/bin/perl

# return couples sid state for all jobs idle and running

$command = "bjobs -u all -w |";
open (CONQ , $command );

# skip first line
$_ = <CONQ>;

while ($_ = <CONQ>) {
    if ( $_ =~ m/test/ ) {
	if ( $` =~ /(\d+)\s+\w+\s+(\w+).*/ ) {
	    if ( $2 == "RUN" ) {
		print "$1 R\n";
	    } else {
		print "$1 I\n";
	    }
	}
    }
}

close(CONQ);
