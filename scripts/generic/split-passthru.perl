#!/usr/bin/env perl

use warnings;
use strict;

if (scalar(@ARGV) > 0 && ($ARGV[0] eq "-train")) {
  exit();
}

system("cat <&0");