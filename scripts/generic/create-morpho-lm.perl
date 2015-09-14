#!/usr/bin/env perl

use strict;
use warnings;
use FindBin qw($RealBin);

binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");

my $arpaPath = $ARGV[0];
my $outDir = $ARGV[1];

open(ARPA_HANDLE, "$arpaPath") or die("Could not open file $arpaPath");
    	
my $unsortedFileName = "tmp.$$.unsorted";
open(my $unsorted, ">$unsortedFileName") or die("Could not open file $unsortedFileName");

while(my $line = <ARPA_HANDLE>) {
	chomp($line);
	my @toks = split('\t', $line);
  #print "line=$line \n";
	
	my $prob;
	my $backoff = 0;
	if (scalar(@toks) >= 2) {
	  # lm record
	  $prob = $toks[0];
	  
	  if (scalar(@toks) == 3) {
	  	# backoff
	  	$backoff = $toks[2];
	  }
	  
	  $line = $toks[1] ." $prob $backoff";
	  print $unsorted "$line\n";
	}	
}
close($unsorted);
close(ARPA_HANDLE);

# sort
my $sortedFileName = "tmp.$$.sorted";
my $cmd = "LC_ALL=C sort $unsortedFileName > $sortedFileName";
`$cmd`;

$cmd = "$RealBin/../../bin/processMorphoLMBin $sortedFileName $outDir";
`$cmd`;
