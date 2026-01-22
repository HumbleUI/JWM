#! /usr/bin/env python3
import argparse, build_utils, common, os, sys, shutil, package

def main() -> int:
  jars = package.main()
  os.chdir(common.basedir)

  build_utils.release_notes(common.version)

  build_utils.collect_jars('io.github.humbleui', 'jwm', common.version, jars, 'target/release')
  return build_utils.release(f"jwm.zip", 'target/release')

if __name__ == "__main__":
  sys.exit(main())
