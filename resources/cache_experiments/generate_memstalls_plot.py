#!/usr/bin/python

import sys
import getopt
import glob

def getPercentage(reportRoot, target, size):
  fileList = glob.glob(reportRoot + "/" + target + "." + size + ".mem")
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
  memstalls = float(cols[6])
  cycles = float(cols[7])
  percentage = memstalls / cycles * 100
  #print "target = %s, size = %s, memstalls = %f, cycles = %f" % (target, size, memstalls, cycles)
  return percentage

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:",["inputDir=","outputFile="])
  except getopt.GetoptError:
    print 'generate_memstalls_plot.py -i <input directory> -o <output file>'
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
  sizes = ["100", "500", "1000", "5000", "10000", "50000", "100000", "500000", "1000000"]

  f = open(fileName,"w")
  f.write("%10s " % "#Elements")
  for target in targets:
    f.write("%10s " % target)
  f.write('\n')
  for size in sizes:
    f.write("%10s " % size)
    for target in targets:
      ipc = getPercentage(reportRoot, target, size)
      f.write("%10.2f " % ipc)
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()
