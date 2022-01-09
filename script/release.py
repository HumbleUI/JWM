#! /usr/bin/env python3
import argparse, build_utils, common, os, sys, shutil, package

def main() -> int:
  jars = package.main()
  os.chdir(common.basedir)

  for jar in jars:
    build_utils.deploy(jar, tempdir = "target/deploy")

  return build_utils.release()

if __name__ == "__main__":
  sys.exit(main())
