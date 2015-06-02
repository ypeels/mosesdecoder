#!/usr/bin/env perl

use warning;
use strict;

if ($ARGV[0] eq "-train") {
  exit();
}

system("cat <&0");