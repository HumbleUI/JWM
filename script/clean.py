#! /usr/bin/env python3
import common, os, shutil, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  shutil.rmtree('shared/target', ignore_errors = True)
  shutil.rmtree(common.system + '/build', ignore_errors = True)
  shutil.rmtree(common.system + '/target', ignore_errors = True)
  shutil.rmtree('examples/target', ignore_errors = True)
  return 0

if __name__ == '__main__':
  sys.exit(main())