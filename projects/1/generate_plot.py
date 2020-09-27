#!/usr/bin/python

import sys
import getopt
import glob

def getIPC(reportRoot, traceName, confName):
  fileList = glob.glob(reportRoot + "/" + traceName + "." + confName + ".out")
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return 0

  while True:
    l = f.readline()
    if l.find("IPC") != -1:
      cols = l.split(":")
      ipc = float(cols[1].replace("\n","").replace(" ", ""))
      break
    if len(l)==0:
      break
  f.close
  return ipc

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

  traceNames = ["sample1", "sample2", "sample3", "sample4"]
  confWidths = ["1-wide", "2-wide"]
  # Order this from least to most optimized
  confOpts = ["no-opt", "data-opt", "data-control-opt", "data-control-mem-opt", "data-control-mem-reg-opt"]

  f = open(fileName,"w")
  f.write("%15s " % "Benchmarks")
  for opt in confOpts:
    f.write("%s " % opt)
  f.write('\n')
  for trace in traceNames:
    for width in confWidths:
      f.write("%15s " % (trace + "." + width))
      ipc_prev = 0
      for opt in confOpts:
        ipc = getIPC(reportRoot, trace, width + "-" + opt)
        ipc_delta = ipc - ipc_prev
        formatString = "%" + str(len(opt)) + ".4f "
        f.write(formatString % ipc_delta)
        ipc_prev = ipc
      f.write('\n')
  f.close

if __name__ == "__main__":
  main()
