#! /usr/bin/env python3
import os, shutil, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  shutil.rmtree('build', ignore_errors = True)
  return 0

if __name__ == '__main__':
  sys.exit(main())