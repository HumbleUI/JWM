#! /usr/bin/env python3
import argparse, build_utils, common, glob, os, platform, subprocess, sys

def build_native():
  os.chdir(common.basedir + "/" + build_utils.system)
  subprocess.check_call(["cmake",
    "-DCMAKE_BUILD_TYPE=Release",
    "-B", "build",
    "-G", "Ninja",
    "-DJWM_ARCH=" + build_utils.arch,
    *(["-DCMAKE_OSX_ARCHITECTURES=" + {"x64": "x86_64", "arm64": "arm64"}[build_utils.arch]] if build_utils.system == "macos" else [])])
  subprocess.check_call(["ninja"], cwd = "build")
  return 0

def build_java():
  os.chdir(common.basedir)
  sources = build_utils.files("linux/java/**/*.java", "macos/java/**/*.java", "shared/java/**/*.java",  "windows/java/**/*.java",)
  build_utils.javac(sources, "target/classes", classpath=common.deps_compile())
  return 0

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--only', choices = ['native', 'java'])
  (args, _) = parser.parse_known_args()
  res = 0
  if None == args.only or 'native' == args.only:
    res += build_native()
  if None == args.only or 'java' == args.only:
    res += build_java()
  return res

if __name__ == '__main__':
  sys.exit(main())
