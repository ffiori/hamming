#!/bin/sh

echo "now single..."
time sh test-all.sh 2>&1 | tee results-single.txt

cd multi
echo "now married lol..."
time sh test-all.sh 2>&1 | tee results-multi.txt
cd ..

