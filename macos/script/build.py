#! /usr/bin/env python3
import os, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  subprocess.check_call(["cmake", "-B", "build", "-G", "Ninja"])
  os.chdir("build")
  subprocess.check_call(["ninja"])
  return 0

if __name__ == '__main__':
  sys.exit(main())