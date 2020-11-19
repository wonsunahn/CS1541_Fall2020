#!/usr/bin/python

import sys
import getopt
import glob

def getTime(reportRoot, prefix, size, metric):
  fileList = glob.glob(reportRoot + "/" + prefix + ".2." + size + ".prof")
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return 0

  while True:
    l = f.readline()
    if l.find(metric) != -1:
      cols = l.split()
      ret = cols[-1].replace("%","").replace("GB/s", "")
      if ret.find("MB/s") != -1:
        ret = ret.replace("MB/s", "")
        ret = str(float(ret) / 1000)
      break
    if len(l)==0:
      break
  f.close
  return ret

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:p:",["inputDir=","outputFile=","filePrefix="])
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
    elif opt in ("-p", "--filePrefix"):
       filePrefix = arg

  sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
  metrics = ["l1_cache_global_hit_rate", "l2_l1_read_hit_rate", "l2_read_throughput"]

  f = open(fileName,"w")
  f.write("%15s " % "BlockSizes")
  for metric in metrics:
    f.write("%s " % metric)
  f.write('\n')
  for size in sizes:
    f.write("%15d " % size) 
    for metric in metrics:
      time = getTime(reportRoot, filePrefix, str(size), metric)
      formatString = "%" + str(len(metric)) + "s "
      f.write(formatString % time)
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()
