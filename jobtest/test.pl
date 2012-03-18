#! /usr/bin/perl

$sec = $ARGV[0];
print "waiting $sec/10 seconds\n";

$| = 1;
$i = 0;
while ( $i < $sec ) {
  select (undef, undef, undef, 0.1);
  $i++;
  print "counter $i\n";
}
print "done\n";


