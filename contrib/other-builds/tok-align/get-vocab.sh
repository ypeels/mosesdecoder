#!/usr/bin/env bash

zcat $1 |  tr ' ' '\n' | LC_ALL=C sort -T . | uniq > $2


