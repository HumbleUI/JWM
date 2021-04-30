#! /usr/bin/env python3
import common, glob, os, platform, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  classpath = [
    common.fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    common.fetch_maven('org.jetbrains', 'annotations', '19.0.0')
  ]
  sources = glob.glob('src/*.java') + glob.glob('src/impl/*.java')
  common.javac(classpath, sources, 'target/classes')
  return 0

if __name__ == '__main__':
  sys.exit(main())
