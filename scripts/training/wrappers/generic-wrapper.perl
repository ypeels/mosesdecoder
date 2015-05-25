#!/usr/bin/env perl

# turns last 2 args into stdin and stdout

use strict;
use warnings;

my $cmd = "";
my $size = scalar(@ARGV);

for (my $i = 0; $i < $size - 2; ++$i) {
  $cmd .= $ARGV[$i] ." ";
}

$cmd .= " < " .$ARGV[$size-2] ." > " .$ARGV[$size-1];
print STDERR "Executing: $cmd\n";
system($cmd);
