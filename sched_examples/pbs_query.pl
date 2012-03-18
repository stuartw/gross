#! /usr/bin/perl

# return couples sid state for all jobs idle and running

$command = "qstat |";
open (CONQ , $command );

# skip first 2 lines
$_ = <CONQ>;
$_ = <CONQ>;

while ($_ = <CONQ>) {
    if ( $_ =~ /(\d+\..+)\s+.+\s+\w+\s+.+\s+(\w+)\s+\w+/ ) {
        if ( $2 eq "R" ) {
	    print "$1 R\n";
	} elsif ( $2 eq "Q" || $2 eq "H" ) {
	    print "$1 I\n";
	}
    }
}

close(CONQ);
