#! /usr/bin/perl

# return couples sid state for all jobs idle and running

$command = "ps -axww | grep \"jobExecutor\" | ";
open (PS , $command );

while (<PS>) {
  if($_ =~ /(\d+)\s+.+\s+\w+\s+.+\s+(.*)jobExecutor\s+(\d+).+\n/) {
      print "${2}jobExecutor::${3}::${1} R\n";
  }
}


