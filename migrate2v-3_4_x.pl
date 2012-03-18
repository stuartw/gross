#!/usr/bin/perl

$mysqlfile = "/tmp/migrate2v-3_4_x.sql";
open (SQL, ">$mysqlfile") || die "Unable to write to file $mysqlfile";
print SQL "ALTER TABLE JOB \n";
print SQL "ADD COLUMN IN_FILES BLOB NOT NULL DEFAULT \"\",\n";
print SQL "ADD COLUMN OUT_FILES BLOB NOT NULL DEFAULT \"\",\n";
print SQL "ADD COLUMN T_LAST INT NOT NULL DEFAULT 0;\n";
print SQL "ALTER TABLE SCHEDULER\n";
print SQL "ADD COLUMN COPY_COMMAND VARCHAR(100) BINARY NOT NULL DEFAULT \"NONE\";\n";
close SQL;

$bossdir = $ENV{"BOSSDIR"};
$cladfile = "${bossdir}/BossConfig.clad";
if ( -f $cladfile ) {
	%classad = &parseClassAd($cladfile);
	$host = $classad{"BOSS_DB_HOST"};
	$db   = $classad{"BOSS_DB_NAME"};
	$user = $classad{"BOSS_DB_USER"};
	$pw   = $classad{"BOSS_DB_USER_PW"};
	print STDERR "Updating database $db \n         on $host\n         using user $user\n         with password $pw\n";
	$command = "mysql -h ${host} -u ${user} --password=${pw} -D $db < ${mysqlfile}";
	system($command);
} else {
	print STDERR "$cladfile not fount \n";
}

unlink $mysqlfile;

sub parseClassAd {
    my $cladfile=$_[0];
    my $cladstring="";
    open (CLAD, $cladfile);
    while ( <CLAD> ) {
	$line = $_;
	$line =~ s/\#.*\n//g; # remove comments
	chomp($line);
	    $cladstring.=$line;
    }
    close(CLAD);
    if ( $cladstring =~ /.*\[(.+)\].*/ ) {
	$cladstring=$1;
    }
    my @attribs = split(/;/,$cladstring);
    foreach $attrib (@attribs) {
	if ( $attrib =~ /\s*(\w+)\s*=\s*(.+)/ ) {
	    $key = $1;
	    $val = $2;
	    $val =~ s/\"//g;
	    $clad{$key}=$val;
	}
    }
    return %clad;
}
