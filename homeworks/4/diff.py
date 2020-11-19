#!/usr/bin/python

import sys
import getopt
import glob

def getFloats(fileName):
  f = open(fileName,"r")
  if f==0:
    print "Unable to open " + fileName
    sys.exit()
  l = f.readline()
  cols = l.split()
  floats = []
  for col in cols:
    try: 
      floats.append(float(col))
    except:
      print "Diff failure!"
      print fileName + " contains spurious output other than the matrix elements."
      sys.exit(-1)
  f.close
  return floats

def getDiff(fileName1, fileName2):
  floats1 = getFloats(fileName1)
  floats2 = getFloats(fileName2)
  diffs = []
  for i in range(len(floats1)):
    diffs.append(floats1[i] - floats2[i])
  return diffs

def main():
  if len(sys.argv) != 3:
    print "diff.py <file1> <file2>"
    sys.exit()
    
  fileName1 = sys.argv[1]
  fileName2 = sys.argv[2]
  diffs = getDiff(fileName1, fileName2)

  diffSuccess = True
  diffString = ""
  for diff in diffs:
    if diff > 0.001 or diff < -0.001:
      diffSuccess = False
    diffString += "%10.6f " % diff

  if diffSuccess == False:
    print "Diff failure!"
    f = open(fileName1,"r")
    print "[" + fileName1 + "]"
    print f.read().replace("\n","")
    f.close()
    f = open(fileName2,"r")
    print "[" + fileName2 + "]"
    print f.read().replace("\n","")
    f.close()
    print "[diff]"
    print diffString 
    sys.exit(-1)
  else:
    print "Diff success!"

if __name__ == "__main__":
  main()
