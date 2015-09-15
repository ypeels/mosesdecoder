#!/usr/bin/env perl

use strict;
use warnings;
use FindBin qw($RealBin);

binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");

my $arpaPath = $ARGV[0];
my $outDir = $ARGV[1];

my $cmd = "mkdir -p $outDir";
`$cmd`;

open(ARPA_HANDLE, "$arpaPath") or die("Could not open file $arpaPath");
    	
my $unsortedFileName = "$outDir/tmp.$$.unsorted";
open(my $unsorted, ">$unsortedFileName") or die("Could not open file $unsortedFileName");

my $vocabPathUnsorted = "$outDir/tmp.$$.vocab.dat.unsorted";
open(my $vocabUnsorted, ">$vocabPathUnsorted") or die("Could not open file $vocabPathUnsorted");

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

	  # vocab
	  my @words = split(' ', $toks[1]);
	  for (my $i = 0; $i < scalar(@words); ++$i) {
	    print $vocabUnsorted $words[$i] ."\n";
  	}

	}		
}
close($unsorted);
close(ARPA_HANDLE);
close($vocabUnsorted);

# vocab
$cmd = "LC_ALL=C sort $vocabPathUnsorted | uniq > $outDir/vocab.dat";
`$cmd`;

# sort
my $sortedFileName = "$outDir/tmp.$$.sorted";
$cmd = "LC_ALL=C sort $unsortedFileName > $sortedFileName";
`$cmd`;

$cmd = "$RealBin/../../bin/processMorphoLMBin $sortedFileName $outDir";
print STDERR "Executing: $cmd\n";
`$cmd`;
