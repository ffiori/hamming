#!/bin/sh

results_file="results-ans2vsans2b-m16-stats.txt"

#~ regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
echo "k vs Total number of instructions executed"
regexp='s/^\([0-9]\{1,30\}\);;instructions.*/\1 /p'
nums=$(sed -n '/ans2 /,/ans2b-noif/p' $results_file | sed -n -e "$regexp")
k=1
for i in $nums; do
	printf "%d %d \n" $k $i;
	k=$(( k+1 ))
done

echo "k vs Percentage of misses of all branches"
regexp='s/^.*;\([0-9]\{1,3\}\.[0-9]\{2\}\);of all branches.*/\1 /p'
nums=$(sed -n '/ans2 /,/ans2b-noif/p' $results_file | sed -n -e "$regexp")
k=1
for i in $nums; do
	printf "%d %.2f \n" $k $i;
	k=$(( k+1 ))
done

echo "k vs occurrences"
regexp='s/^.*ans2 .*occ=\([0-9]\{1,30\}\).*/\1 /p'
nums=$(sed -n '/ans2 /,/ans2b-noif/p' $results_file | sed -n -e "$regexp")
k=1
for i in $nums; do
	printf "%d %d \n" $k $i;
	k=$(( k+1 ))
done
