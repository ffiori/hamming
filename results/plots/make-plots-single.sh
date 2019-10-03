#!/bin/sh

rm plot_single*.eps;
rm plot_single*.tex;

for file in plot_single_*; do gnuplot $file; done
