#!/usr/bin/env sh

# Put backslashes in paths but not switches, make paths absolute (pretty sure Wine uses Z: for the root)
args=""
for i in $*; do
	if [ -e $i ]; then
		args="$args Z:$(realpath $i | sed 's/\//\\/g')"
	else
		args="$args $i"
	fi
done

cmd="wine $(dirname $0)/tools/$(basename $0) $args"
echo $cmd
$cmd

