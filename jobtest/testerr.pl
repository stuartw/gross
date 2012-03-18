#! /usr/bin/perl

$sec = $ARGV[0];
print "waiting $sec seconds\n";

$| = 1;
$i = 0;
while ( $i < $sec ) {
  sleep(1);
  $i++;
  print "Outcounter $i\n";
  if( int($i/10)*10 eq $i ) {
      print STDERR "Errcounter $i\n";
  }
}
print "done\n";


