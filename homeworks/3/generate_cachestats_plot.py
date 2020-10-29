#!/usr/bin/python

import sys
import getopt
import glob

l1missrate = 0
l2missrate = 0
l3missrate = 0

def getPercentage(reportRoot, target, size):
  global l1missrate
  global l2missrate
  global l3missrate

  fileList = glob.glob(reportRoot + "/" + target + "." + size + ".cache")
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return

  lastLine = ""
  while True:
    l = f.readline()
    if len(l)==0:
      break
    else:
      lastLine = l
  f.close

  cols = lastLine.split()
  l3misses = float(cols[2])
  l3loads = float(cols[3])
  l2misses = float(cols[4])
  l2hits = float(cols[5])
  l1misses = float(cols[6])
  l1loads = float(cols[7])

  # print "target = %s, size = %s, l3misses = %f, l2misses = %f, l1misses=%f " % (target, size, l3misses, l2misses, l1misses)

  if l3loads > 0:
    l3missrate = l3misses / l3loads * 100
  else:
    l3missrate = 0
  if l2misses + l2hits > 0:
    l2missrate = l2misses / (l2misses + l2hits) * 100
  else:
    l2missrate = 0
  if l1loads > 0:
    l1missrate = l1misses / l1loads * 100
  else:
    l1missrate = 0

  return

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:t:",["inputDir=","outputFile=","targetName-"])
  except getopt.GetoptError:
    print 'generate_cachestats_plot.py -i <input directory> -o <output file> -t <target name>'
    sys.exit(1)
  for opt, arg in opts:
    if opt == '-h':
      print 'generate_cachestats_plot.py -i <input directory> -o <output file> -t <target name>'
      sys.exit()
    elif opt in ("-i", "--inputDir"):
      reportRoot = arg
    elif opt in ("-o", "--outputFile"):
      fileName = arg
    elif opt in ("-t", "--targetName"):
      target = arg

  targets = ["linked-list", "linked-list2"]
  sizes = ["50", "100", "500", "1000", "5000", "10000", "50000", "100000", "500000", "1000000", "5000000", "10000000"]

  f = open(fileName,"w")
  f.write("%10s " % "#Elements")
  f.write("%10s %10s %10s" % ("L1MissRate", "L2MissRate", "L3MissRate"))
  f.write('\n')
  for size in sizes:
    f.write("%10s " % size)
    getPercentage(reportRoot, target, size)
    f.write("%10.2f %10.2f %10.2f" % (l1missrate, l2missrate, l3missrate))
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()
