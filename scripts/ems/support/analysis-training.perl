#!/usr/bin/env perl

use strict;
use warnings;
use Getopt::Long "GetOptions";
use File::Basename;
 
#GetOptions('source=s' => \$sourcePath,
#					'target=s' => \$targetPath
#					);

for (my $i = 0; $i < scalar(@ARGV); ++$i) {
	my $path = $ARGV[$i];
  my ($basename, $numToks, $numTypes) = Calc($path);
  my $ttr = $numToks / $numTypes;
  print "$basename: $numToks $numTypes $ttr \n";
}

############################################
sub Calc
{
  # tokens
	my $dataPath = shift;
	my $basename = basename($dataPath);
	
	my $cmd = "cat $dataPath | wc -w";
	my $numToks = `$cmd`;
	chomp($numToks);
	
	# types
	my $tmpDir = dirname($dataPath);
	
  $cmd = "cat $dataPath  | tr ' ' '\\n' | LC_ALL=C sort -T $tmpDir | uniq | wc -w";
	my $numTypes = `$cmd`;
	chomp($numTypes);

	return ($basename, $numToks, $numTypes);
}
