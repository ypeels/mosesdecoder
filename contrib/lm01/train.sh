#!/bin/sh

DIR="$( cd "$( dirname "$0" )" && pwd )"

$DIR/extract $1 $2 $3 extracted.gz
zcat extracted.gz | LC_ALL=C sort -T . | gzip -c > extracted.sorted.gz
$DIR/score extracted.sorted.gz $4

