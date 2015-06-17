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

my $outStem = $ARGV[0];
my $inExt = $ARGV[1];
my $outExt = $ARGV[2];

my $corpusStem = $ARGV[3];
my $cmd;

safesystem("rm -rf $outStem.$inExt");
safesystem("rm -rf $outStem.$outExt");

CalcParallelCorpusStat($corpusStem, $inExt, $outExt, $outStem);

# target
$cmd = "$RealBin/calc-oov --method " .$methodTarget 
				. " --output-oov " .$outputOOVTarget;
$cmd .= " --juncture \"" .$juncture ."\"";
$cmd .= " $corpusStem.$outExt ";
				
for (my $i = 4; $i < scalar(@ARGV); ++$i) {
  my $testPath = $ARGV[$i];
  $cmd .= "$testPath "
}

$cmd .= ">> $outStem.$outExt";
safesystem($cmd);

#########################################
sub CalcParallelCorpusStat {
  my ($corpusStem, $inExt, $outExt, $outStem) = (shift, shift, shift, shift);
  
	CalcCorpusStat("$corpusStem.$inExt", "$outStem.$inExt");
	CalcCorpusStat("$corpusStem.$outExt", "$outStem.$outExt");
}

sub CalcCorpusStat {
  my ($corpusPath, $outPath) = (shift, shift);
  
  print STDERR "corpusPath=$corpusPath\n";
  print STDERR "outPath=$outPath\n";
  
  my $cmd = "cat $corpusPath | wc -w";
  my $numToks = `$cmd`;
  chomp($numToks);
  
  safesystem("echo ---------------------------- >> $outPath");

  $cmd = "cat $corpusPath | tr ' ' '\n' | LC_ALL=C sort | uniq | wc -w";
  my $numTypes = `$cmd`;
  chomp($numTypes);
  
  my $ratio = $numToks / $numTypes;
  
  safesystem("echo Stats for $corpusPath >> $outPath");
  $cmd = "echo numToks=$numToks numTypes=$numTypes ratio=$ratio >> $outPath";
  safesystem($cmd);
}

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


