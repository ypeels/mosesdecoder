#!/usr/bin/env perl

use warnings;
use strict;
use FindBin qw($RealBin);

my $cmd = "$RealBin/clean-corpus-n.perl " .$ARGV[1] ." " .$ARGV[2] ." " .$ARGV[3] ." " .$ARGV[4] ." " .$ARGV[5] ." " .$ARGV[6] ." " .$ARGV[7];
print STDERR "Executing: $cmd\n";
`$cmd`;
