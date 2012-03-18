#! /usr/bin/perl

$len=@ARGV;
if($len != 1) {
  print "error";
  die "Kill: Wrong number of arguments";
}
  
if ($ARGV[0] =~ /(.+)::(\d+)::(\d+)/ ) {
  $executable=$1;
  $jid = $2;
  $pid = $3;
  $status = "error";
  # jobExecutor was submit with a sh -c command, first find 
  # the real pid if jobExecutor, the children of sh
  $command = "ps -a h -o 'pid ppid' | grep \"$pid\" | ";
  open (PS , $command);
  $job_found=0;
  while (<PS>) {
      if($_ =~ /(\d+)\s+(\d+)\n/) {
	  # kill children
	  if ($2 == $pid) {
	      $child=$1;
	      $job_found=1;
	  }   
      }
  }
  close(PS);
  # kill all children and then jobExecutor 
  if ( $job_found == 1 ) { 
    #  print "$child\n";
      $command = "ps -a h -o 'pid ppid' | grep \"$child\" | ";
      open (PS , $command );
      while (<PS>) {
	  if($_ =~ /(\d+)\s+(\d+)\n/) {
	      # kill children
	      if ($2 == $child) {
		#  print "killing $1\n";
		  system("kill -9 $1");
		  $status="killed";
	      }
	      # kill father
	      if ($1 == $child) {
		#  print "killing $1\n";
		  system("kill -9 $1");
		  $status="killed";
	      }      
	  }
      }
  } else {
      $status="job not found error";
  }
} else { 
    $status="error";
}
print "$status\n";



