#! /usr/bin/env python3
import argparse, common, glob, os, platform, subprocess, sys

target_native = {'macos': 'libjwm_' + common.arch + '.dylib',
                 'linux': 'libjwm_' + common.arch + '.so',
                 'windows': 'jwm_' + common.arch + '.dll'}[common.system]

def build_native():
  print('Building ' + target_native + '...')
  print(common.system)
  subprocess.check_call([
    "cmake",
    "-DCMAKE_BUILD_TYPE=Release",
    "-B", "build",
    "-G", "Ninja",
    "-DJWM_ARCH=" + common.arch,
  ] + (["-DCMAKE_OSX_ARCHITECTURES=" + {"x64": "x86_64", "arm64": "arm64"}[common.arch]] if common.system == "macos" else []),
  cwd = common.system)
  subprocess.check_call(["ninja"], cwd = common.system + "/build")

def build_java():
  os.chdir(os.path.dirname(__file__) + '/..')
  sources = glob.glob('linux/java/**/*.java', recursive=True) + \
            glob.glob('macos/java/**/*.java', recursive=True) + \
            glob.glob('shared/java/**/*.java', recursive=True) + \
            glob.glob('windows/java/**/*.java', recursive=True) + \
            glob.glob('examples/java/*.java', recursive=True)
  print('Building java classes...')
  common.javac(common.deps(), sources, 'target/classes')

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('--only', choices = ['native', 'java', 'examples'])
  (args, _) = parser.parse_known_args()
  if None == args.only or 'native' == args.only:
    build_native()
  if None == args.only or 'java' == args.only:
    build_java()


  sys.exit(0)
