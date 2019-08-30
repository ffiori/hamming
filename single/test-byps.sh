#!/bin/sh

p=100
e=100
q=4
T=60000

echo "Preprocessing runs $p. Search runs $e. q-gram for EF and EFS $q."

for text in ../multiple/tests/genome/ecoli.txt ../multiple/tests/english/bible.txt; do
      echo "\n\n********** $text **********\n"
      for m in 8 16 24 32; do
        echo "\nPattern length of $m:"
        for k in 1 2 3; do
              echo "for $k mismatches:"

              ./byp/byps-dna/byps -e$e -k$k -p$p $text < $text.$m.n100.badpat

              echo ""
        done
    done
done

# sh test-all.sh 2>&1 | tee results-all.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt

