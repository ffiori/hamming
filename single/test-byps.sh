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

              ./byp/bypsb/bypsb-memcmp -e$e -k$k -p$p $text < $text.$m.n100.badpat
              ./byp/bypsb/bypsb-simdcmp16 -e$e -k$k -p$p $text < $text.$m.n100.badpat
              ./byp/bypsb/bypsb-simdcmp32 -e$e -k$k -p$p $text < $text.$m.n100.badpat
              ./byp/bypsb/bypsb-nocomp -e$e -k$k -p$p $text < $text.$m.n100.badpat

              echo ""
        done
    done
done

# sh test-byps.sh 2>&1 | tee results-bypsb-19-10-04.txt

# to run in background (final &) even if terminal closes (nohup)
# nohup sh test-all.sh > results.txt 2>&1 &
# nohup sh test-all.sh 2>&1 | tee results8.txt

