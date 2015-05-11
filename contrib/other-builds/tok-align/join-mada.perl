#!/usr/bin/env perl

use strict;
use warnings;


binmode(STDIN, ":utf8");
binmode(STDOUT, ":utf8");

sub ProcessLine;

while (my $line = <STDIN>) {
  chomp($line);
	ProcessLine($line);

}


#####################
sub ProcessLine
{
  my $line = shift;
	my @toks = split(/ /, $line);
	
	my $prevPrefix = 1;
	for (my $i = 0; $i < scalar(@toks); ++$i) {
	  my $prefix = 0;
	  my $suffix = 0;
	  my $tok = $toks[$i];
		if (length($tok) <= 1) {
		  # do nothing
		} 
		elsif (substr($tok, length($tok) -1, 1) eq "+") {
		  $prefix = 1;
		  $tok = substr($tok, 0, length($tok) -1);
		}
		elsif (substr($tok, 0, 1) eq "+") {
			$suffix = 1;
		  $tok = substr($tok, 1, length($tok) -1);		
		}
		else {
		  # do nothing
		}
		
		if ($prevPrefix == 0 && $suffix == 0) {
		  print " ";
		}
		print $tok;
		
		$prevPrefix = $prefix;
	}
	
	print "\n";
	
}

