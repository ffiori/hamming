#!/bin/sh

echo "Compiling all algorithms...\n"

for dir in */; do
	echo "Cleaning $dir..."
	cd $dir
	make clean
	cd ..
	echo "...done!\n"
done
