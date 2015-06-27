#!/usr/bin/env perl

use strict;
use warnings;
use Getopt::Long "GetOptions";
use FindBin qw($RealBin);

my $FACTOR;
my $CONFIG;
my $MORFDIR;
my $LC = 0;

GetOptions(
  "factor=i" => \$FACTOR,
  "config=s" => \$CONFIG,
  "morfessor-dir=s" => \$MORFDIR,
  "lowercase=i" => \$LC
    ) or die("ERROR: unknown options");

die("must provide -factor and -config args") if !defined($FACTOR) or !defined($CONFIG) or !defined($MORFDIR);

my $INPATH = $ARGV[0];
my $OUTPATH = $ARGV[1];
my $TMPDIR = $ARGV[2];
my $MOSESDIR = "$RealBin/../../../";
my $cmd;

print STDERR "LC=$LC\n";

my $tmp1;
if ($LC) {
	$tmp1 = "$TMPDIR/tmp1.$$";
	$cmd = "$MOSESDIR/scripts/tokenizer/lowercase.perl < $INPATH > $tmp1";
	safesystem($cmd);
}
else {
	$tmp1 = $INPATH;
}

# run morfessor
my $tmp2 = "$TMPDIR/tmp2.$$";
$cmd = "$MOSESDIR/scripts/generic/morfessor-wrapper.perl --morfessor-dir $MORFDIR -model $CONFIG < $tmp1 > $tmp2";
safesystem($cmd);

#add juncture to target side of corpus
my $tmp3 = "$TMPDIR/tmp3.$$";
$cmd = "$MOSESDIR/contrib/other-builds/tok-align/tok-align $tmp2 $tmp1 --method 2 --junctured-path $tmp3 > /dev/null";
safesystem($cmd);

# MAIN LOOP
open(INFILE, $tmp3) or die("Can't open file $tmp3");
open(OUTFILE, ">$OUTPATH") or die("Can't open file $OUTPATH");

while (my $line = <INFILE>) {
	chomp($line);
	my @toks = split(/\s/,$line);
	
	my $fullWord = "";
	my $stem = "";
	my $suffix = "";
	for (my $i = 0; $i < scalar(@toks); ++$i) {
		my $tok = $toks[$i];
		my $end = CreateFactors(\$fullWord, \$stem, \$suffix, $tok);
		
		if ($end == 0) {
			if ($suffix eq  "") {
			  $suffix = "$fullWord";
			}
			else {
				$suffix = "+$suffix";
			}
			
			if ($FACTOR == 0) {
	  		print OUTFILE "$fullWord ";
	  	}
			elsif ($FACTOR == 1) {
	  		print OUTFILE "$stem ";
	  	}
			elsif ($FACTOR == 2) {
	  		print OUTFILE "$suffix ";
	  	}
			else {
				die("Unknown factor $FACTOR");
			}
				  	
			$stem = "";
			$fullWord = "";
			$suffix = "";
		}
	}
	
	print OUTFILE "\n";
}

close(INFILE);
close(OUTFILE);

############################################
sub CreateFactors {
	my $fullWord = shift;
	my $stem = shift;
	my $suffix = shift;
	my $tok = shift;
	
	my $begin = 0;
	my $end = 0;
	
  #print STDERR "tok=$tok ";
	
	my $size;

	$size = length($tok);
	if ($size > 1 && substr($tok, 0, 1) eq "+") {
		# beginning juncture
		#print STDERR "BEGIN";
		$begin = 1;
		$tok = substr($tok, 1, $size - 1);		
	}
	else {
		$$stem = $tok;
	}

	$size = length($tok);
  if ($size > 1 && substr($tok, $size - 1, 1) eq "+") {
  	#end juncture
		#print STDERR "END";	
		$end = 1;
		
		$tok = substr($tok, 0, $size - 1);
	}
		
	if ($begin == 1) {
	  $$suffix .= $tok;
  }
  
	$$fullWord .= $tok;
	
	return $end;
}


sub safesystem {
  print STDERR "Executing: @_\n";
  system("bash", "-c", @_);
  if ($? == -1) {
      print STDERR "Failed to execute: @_\n  $!\n";
      exit(1);
  }
  elsif ($? & 127) {
      printf STDERR "Execution of: @_\n  died with signal %d, %s coredump\n",
          ($? & 127),  ($? & 128) ? 'with' : 'without';
      exit(1);
  }
  else {
    my $exitcode = $? >> 8;
    print STDERR "Exit code: $exitcode\n" if $exitcode;
    return ! $exitcode;
  }
}

