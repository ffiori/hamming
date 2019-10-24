#!/bin/sh

results_file="../results-bypsb-19-10-04.txt"


echo "% DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA\n"

for alg in "bypsb-simdcmp16" "bypsb-nocomp"; do

printf "%-15s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n -e "$regexp")
for i in $nums; do printf "{%4.2f} & " $i; done
echo "\\\\\\"

done


echo "\n% English English English English English English English English English \n"

for alg in "bypsb-simdcmp16" "bypsb-nocomp"; do

printf "%-15s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n -e "$regexp")
for i in $nums; do printf "{%4.2f} & " $i; done
echo "\\\\\\"

done
