# Plot configurations

set style line 1 lt rgb "red" lw 20 ps 5
set style line 2 lt rgb "blue" lw 20 ps 5
set style line 3 lt rgb "purple" lw 20 ps 5
set title "Memory profile of mat-vec multiply on GPU"
set xlabel "Thread block sizes" 
set ylabel "Hit rate (%)"
set y2label "GB/s"
set ytics nomirror
set y2tics 0, 2
set xrange [ * : * ] noreverse writeback
set x2range [ * : * ] noreverse writeback
set yrange [ * : 100 ] noreverse writeback
set y2range [ * : * ] noreverse writeback
set cbrange [ * : * ] noreverse writeback
set rrange [ * : * ] noreverse writeback
set logscale x
set key at graph 1.0, 0.7

# Render plot
set terminal pdf transparent enhanced font "arial,10" fontscale 5.0 size 45, 15

set output outputFile
plot inputFile using 1:2:xticlabels(1) with linespoints linestyle 1 t "L1 Hit Rate", \
inputFile using 1:3:xticlabels(1) with linespoints linestyle 2 t "L2 Hit Rate", \
inputFile using 1:4:xticlabels(1) with linespoints linestyle 3 t "DRAM Transfer Rate" axis x1y2
unset out

