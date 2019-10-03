#!/bin/sh

rm plot_multiple*.eps;
rm plot_multiple*.tex;

for file in plot_multiple_*; do gnuplot $file; done
