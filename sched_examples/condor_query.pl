#! /usr/bin/perl

# return couples sid state for all jobs idle and running

$command = "condor_q -global -constraint \"BossJob >0\" |";
open (CONQ , $command );

# skip first four line
$_ = <CONQ>;
$_ = <CONQ>; 
$_ = <CONQ>; 
$_ = <CONQ>; 

while (<CONQ>) {
if ( $_ =~ /(\d+).\d+\s+\w+\s+.+\s+.+\s.+\s+(\w+)\s+\d+\s+.+\sjobExecutor\s+\d+/ ) {
      print "$1 $2\n";
  }
}


