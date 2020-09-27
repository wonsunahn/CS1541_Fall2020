# Plot configurations

set border 3 front lt black linewidth 1.000 dashtype solid
set boxwidth 0.8 absolute
set style fill   solid 1.00 noborder
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics nortics nomrtics \
 nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault   lt 0 linecolor 0 linewidth 0.500,  lt 0 linecolor 0 linewidth 0.500
set key bmargin center horizontal Left reverse noenhanced autotitle columnhead nobox
set style increment default
set style histogram rowstacked title textcolor lt -1 offset character 2, 0.25
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror rotate by -45  autojustify
set xtics  norangelimit  font ",8"
set xtics   ()
set ytics border in scale 0,0 mirror norotate  autojustify
set ytics  norangelimit autofreq  font ",8"
set ztics border in scale 0,0 nomirror norotate  autojustify
set cbtics border in scale 0,0 mirror norotate  autojustify
set rtics axis in scale 0,0 nomirror norotate  autojustify
set title "IPCs of 4 traces using different processor configrations" 
set xlabel "Width of Processor" 
set xlabel  offset character 0, -2, 0 font "" textcolor lt -1 norotate
set xrange [ * : * ] noreverse writeback
set x2range [ * : * ] noreverse writeback
set ylabel "IPC (Instructions per Cycle)" 
set yrange [ 0.00000 : 2.00000 ] noreverse writeback
set y2range [ * : * ] noreverse writeback
set zrange [ * : * ] noreverse writeback
set cbrange [ * : * ] noreverse writeback
set rrange [ * : * ] noreverse writeback

# Render plot
set terminal pdf transparent enhanced font "arial,10" fontscale 5.0 size 45, 30

if (!exists("inputFile")) filename='IPC.dat'
if (!exists("outputFile")) filename='IPC.pdf'
set output outputFile
plot inputFile using "no-opt":xtic(1) t "no optimization", '' u "data-opt" t "enableForwarding", '' u "data-control-opt" t "branchPredictor", '' u "data-control-mem-opt" t "splitCaches", '' u "data-control-mem-reg-opt" t "regFileWritePorts=2"
unset out

