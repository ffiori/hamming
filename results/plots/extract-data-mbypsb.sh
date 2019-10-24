#!/bin/sh

results_file="../results-mbypsb-19-10-05-i7.txt"


echo "% DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA"

echo "\n10 patterns"
for alg in "mbyps " "mbypsb-simdcmp16" "mbypsb-nocomp"; do
printf "%-16s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e "$regexp")
for i in $nums; do printf "{%4.3f} & " $i; done
echo "& \\\\\\"
done

echo "\n100 patterns"
for alg in "mbyps " "mbypsb-simdcmp16" "mbypsb-nocomp"; do
printf "%-16s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e "$regexp")
for i in $nums; do printf "{%4.3f} & " $i; done
echo "& \\\\\\"
done

echo "\n1000 patterns"
for alg in "mbyps " "mbypsb-simdcmp16" "mbypsb-nocomp"; do
printf "%-16s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e "$regexp")
for i in $nums; do printf "{%4.3f} & " $i; done
echo "& \\\\\\"
done


echo "\n% English English English English English English English English English"

echo "\n10 patterns"
for alg in "mbyps " "mbypsb-simdcmp16" "mbypsb-nocomp"; do
printf "%-16s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e "$regexp")
for i in $nums; do printf "{%4.3f} & " $i; done
echo "& \\\\\\"
done

echo "\n100 patterns"
for alg in "mbyps " "mbypsb-simdcmp16" "mbypsb-nocomp"; do
printf "%-16s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e "$regexp")
for i in $nums; do printf "{%4.3f} & " $i; done
echo "& \\\\\\"
done

echo "\n1000 patterns"
for alg in "mbyps " "mbypsb-simdcmp16" "mbypsb-nocomp"; do
printf "%-16s & " $alg
regexp='s/^.*\/'$alg'.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p'
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e "$regexp")
for i in $nums; do printf "{%4.3f} & " $i; done
echo "& \\\\\\"
done
