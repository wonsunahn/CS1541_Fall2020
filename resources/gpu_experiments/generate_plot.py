#!/usr/bin/python

import sys
import getopt
import glob

def getTime(reportRoot, prefix, policy, size, comp):
  fileList = glob.glob(reportRoot + "/" + prefix + "." + policy + "." + size + ".perf")
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return 0

  while True:
    l = f.readline()
    if l.find(comp) != -1:
      cols = l.split(" ")
      time = float(cols[2].replace("\n","").replace(" ", ""))
      break
    if len(l)==0:
      break
  f.close
  return time

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

  policies = ["1", "2", "3"]
  if filePrefix.find("vec") != -1:
    sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
  else:
    sizes = [1, 2, 4, 8, 16, 32]
  timeComponents = ["Memcpy", "Compute"]

  f = open(fileName,"w")
  f.write("%15s " % "BlockSizes")
  for comp in timeComponents:
    for policy in policies:
      f.write("%s.%s " % (comp, policy))
  f.write('\n')
  for size in sizes:
    if filePrefix.find("vec") != -1:
      f.write("%15d " % size) 
    else:
      f.write("%15d " % (size * size))
    for comp in timeComponents:
      for policy in policies:
        time = getTime(reportRoot, filePrefix, policy, str(size), comp)
        formatString = "%" + str(len(comp) + 2) + ".6f "
        f.write(formatString % time)
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()
