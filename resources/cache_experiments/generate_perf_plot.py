#!/usr/bin/python

import sys
import getopt
import glob

def getIPC(reportRoot, target, size):
  fileList = glob.glob(reportRoot + "/" + target + "." + size + ".perf")
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return 0

  lastLine = ""
  while True:
    l = f.readline()
    if len(l)==0:
      break
    else:
      lastLine = l
  f.close

  cols = lastLine.split()
  insts = float(cols[3])
  cycles = float(cols[4])
  cpi = cycles / insts
  return cpi

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:",["inputDir=","outputFile="])
  except getopt.GetoptError:
    print 'generate_plot.py -i <input directory> -o <output file>'
    sys.exit(1)
  for opt, arg in opts:
    if opt == '-h':
       print 'generate_plot.py -i <input directory> -o <output file>'
       sys.exit()
    elif opt in ("-i", "--inputDir"):
       reportRoot = arg
    elif opt in ("-o", "--outputFile"):
       fileName = arg

  targets = ["array", "array_nodata", "linked-list", "linked-list_nodata"]
  sizes = ["50", "100", "500", "1000", "5000", "10000", "50000", "100000", "500000", "1000000", "5000000", "10000000"]
  #sizes = ["50", "100", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000", "5000", "10000", "50000", "100000", "500000", "1000000", "5000000", "10000000"]

  f = open(fileName,"w")
  f.write("%10s " % "#Elements")
  for target in targets:
    f.write("%10s " % target)
  f.write('\n')
  for size in sizes:
    f.write("%10s " % size)
    for target in targets:
      cpi = getIPC(reportRoot, target, size)
      f.write("%10.2f " % cpi)
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()
