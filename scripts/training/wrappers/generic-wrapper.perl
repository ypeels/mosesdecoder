#!/usr/bin/env perl

# turns last 2 args into stdin and stdout

use strict;
use warnings;

my $cmd = "";
my $size = scalar(@ARGV);

for (my $i = 0; $i < $size - 3; ++$i) {
  $cmd .= $ARGV[$i] ." ";
}

$cmd .= " < " .$ARGV[$size-3] ." > " .$ARGV[$size-2];
print STDERR "Executing: $cmd\n";
system($cmd);
