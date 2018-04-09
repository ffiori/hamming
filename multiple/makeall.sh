#!/bin/sh

echo "Compiling all algorithms...\n"

for dir in */; do
	echo "Compiling $dir..."
	cd $dir
	make
	cd ..
	echo "...done!\n"
done
