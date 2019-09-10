#!/bin/sh

results_file="results-19-08-28.txt"


echo "% DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA\n"

for alg in "sa" "tusa" "twsa" "ans " "ansb" "ans2 " "ans2b " "ans2b-noif" "ef " "efs" "byp " "byps"; do

printf "%-10s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n -e "$regexp")
for i in $nums; do printf "{%5.2f} & " $i; done
echo "\\\\\\"

done


echo "\n% English English English English English English English English English \n"

for alg in "sa" "tusa" "twsa" "ans " "ansb" "ans2 " "ans2b " "ans2b-noif" "byp " "byps"; do

printf "%-10s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n -e "$regexp")
for i in $nums; do printf "{%4.2f} & " $i; done
echo "\\\\\\"

done
