#!/usr/bin/env perl

use warnings;
use strict;
use Getopt::Long "GetOptions";
use FindBin qw($RealBin);

my $methodTarget = 1;
my $outputOOVTarget = 0;
my $juncture = "";

GetOptions(
  "method-target=i" => \$methodTarget,
  "output-oov-target" => \$outputOOVTarget,
  "juncture=s" => \$juncture
) or exit(1);

my $outPath = $ARGV[0];
my $inExt = $ARGV[1];
my $outExt = $ARGV[2];

my $corpusPath = $ARGV[3];

print STDERR "ARGV=" .scalar(@ARGV) ."\n";

# target
my $cmd = "$RealBin/calc-oov --method " .$methodTarget 
				. " --output-oov " .$outputOOVTarget;
$cmd .= " --juncture \"" .$juncture ."\"";
$cmd .= " $corpusPath.$outExt ";
				
for (my $i = 4; $i < scalar(@ARGV); ++$i) {
  my $testPath = $ARGV[$i];
  $cmd .= "$testPath "
}

$cmd .= "> $outPath.$outExt";
safesystem($cmd);

#########################################
sub safesystem {
  print STDERR "Executing: @_\n";
  system("bash", "-c", @_);
  if ($? == -1) {
      print STDERR "Failed to execute: @_\n  $!\n";
      exit(1);
  }
  elsif ($? & 127) {
      printf STDERR "Execution of: @_\n  died with signal %d, %s coredump\n",
          ($? & 127),  ($? & 128) ? 'with' : 'without';
      exit(1);
  }
  else {
    my $exitcode = $? >> 8;
    print STDERR "Exit code: $exitcode\n" if $exitcode;
    return ! $exitcode;
  }
}


