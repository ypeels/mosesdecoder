#!/usr/bin/env perl

use strict;
use warnings;
use Getopt::Long "GetOptions";
use File::Basename;
use Set::Object qw(set);

my $vocab = set();

my $corpusPath;
my $testPath;

GetOptions('corpus=s' => \$corpusPath,
					'test=s' => \$testPath
					);


############################################
sub Calc
{
  # tokens
	my $dataPath = shift;
	my $basename = basename($dataPath);	
	my $tmpDir = dirname($dataPath);
	
	my $cmd;
	
  $cmd = "cat $dataPath  | tr ' ' '\\n' | LC_ALL=C sort -T $tmpDir | uniq > $tmpDir/vocab";
  safesystem($cmd);


}

sub safesystem {
  print STDERR "Executing: @_\n";
  system(@_);
  if ($? == -1) {
    print STDERR "Failed to execute: @_\n  $!\n";
    exit(1);
  }
  elsif ($? & 127) {
    printf STDERR "Execution of: @_\n  died with signal %d, %s coredump\n",
      ($? & 127),  ($? & 128) ? 'with' : 'without';
    exit 1;
  }
  else {
    my $exitcode = $? >> 8;
    print STDERR "Exit code: $exitcode\n" if $exitcode;
    return ! $exitcode;
  }
}

