#!/bin/sh

results_file="results-mbyps-19-08-29.txt"

echo "DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA DNA\n"

echo "10 patterns\n"

printf "%s" "MM & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*udi.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "FN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*cmfn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "BYN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*original\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "HBYN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*hamming\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "MBYPS & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*multi\/mbyps.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "\\\textbf{%.3f} & " $i; done
echo "& \\\\\\"


echo "\n100 patterns\n"

printf "%s" "MM & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*udi.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "FN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*cmfn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "BYN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*original\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "HBYN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*hamming\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "MBYPS & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*multi\/mbyps.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "\\\textbf{%.3f} & " $i; done
echo "& \\\\\\"


echo "\n1000 patterns\n"

printf "%s" "MM & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*udi.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "FN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*cmfn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "BYN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*original\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "HBYN & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*hamming\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "MBYPS & "
nums=$(sed -n '/ecoli/,/english/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*multi\/mbyps.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "\\\textbf{%.3f} & " $i; done
echo "& \\\\\\"


echo "\nEnglish English English English English English English English English English English \n"

echo "10 patterns\n"

printf "%s" "MM & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*udi.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "FN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*cmfn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "BYN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*original\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "HBYN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*hamming\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "MBYPS & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/10 patterns/,/100 patterns/p' | sed -n -e 's/^.*multi\/mbyps.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "\\\textbf{%.3f} & " $i; done
echo "& \\\\\\"


echo "\n100 patterns\n"

printf "%s" "MM & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*udi.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "FN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*cmfn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "BYN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*original\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "HBYN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*hamming\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "MBYPS & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/100 patterns/,/1000 patterns/p' | sed -n -e 's/^.*multi\/mbyps.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "\\\textbf{%.3f} & " $i; done
echo "& \\\\\\"


echo "\n1000 patterns\n"

printf "%s" "MM & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*udi.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "FN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*cmfn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "BYN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*original\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "HBYN & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*hamming\/byn.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "%.3f & " $i; done
echo "& \\\\\\"

printf "%s" "MBYPS & "
nums=$(sed -n '/english/,/ecoli/p' $results_file | sed -n '/1000 patterns/,/10 patterns/p' | sed -n -e 's/^.*multi\/mbyps.*e=\([0-9]\{1,3\}\.[0-9]\{3\}[5-9]\{0,1\}\).*/\1 /p')
for i in $nums; do printf "\\\textbf{%.3f} & " $i; done
echo "& \\\\\\"
