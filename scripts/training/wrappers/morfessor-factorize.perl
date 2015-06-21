#!/usr/bin/env perl

use strict;
use warnings;


while (my $line = <STDIN>) {
	chomp($line);
	my @toks = split(/\s/,$line);
	
	my $stem = "";
	my $fullWord = "";
	for (my $i = 0; $i < scalar(@toks); ++$i) {
		my $tok = $toks[$i];
		my $end = CreateFactors(\$stem, \$fullWord, $tok);
		
		
		if ($end == 0) {
  		print "$stem|$fullWord ";
			$stem = "";
			$fullWord = "";
		}
	}
	
	print "\n";
}

sub CreateFactors {
	my $stem = shift;
	my $fullWord = shift;
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
		
	$$fullWord .= $tok;
	
	return $end;
}