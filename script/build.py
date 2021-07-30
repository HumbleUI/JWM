#! /usr/bin/env python3
import common, glob, os, platform, subprocess, sys

def build_shared():
  os.chdir(os.path.dirname(__file__) + '/..')
  sources = glob.glob('shared/java/**/*.java', recursive=True)
  common.javac(common.deps(), sources, 'shared/target/classes')

target_native = {'macos': 'libjwm_' + common.arch + '.dylib',
                 'linux': 'libjwm_' + common.arch + '.so',
                 'windows': 'jwm_' + common.arch + '.dll'}[common.system]

def build_native():
  build_shared()
  print('Building', target_native)
  subprocess.check_call([
    "cmake",
    "-DCMAKE_BUILD_TYPE=Release",
    "-B", "build",
    "-G", "Ninja",
    "-DJWM_ARCH=" + common.arch,
  ] + (["-DCMAKE_OSX_ARCHITECTURES=" + {"x64": "x86_64", "arm64": "arm64"}[common.arch]] if common.system == "macos" else []),
  cwd = common.system)
  subprocess.check_call(["ninja"], cwd = common.system + "/build")
  classpath = common.deps() + ['shared/target/classes']
  sources = glob.glob(common.system + '/java/**/*.java', recursive=True)
  common.javac(classpath, sources, common.system + '/target/classes')

def main():
  build_native()
  return 0

if __name__ == '__main__':
  sys.exit(main())
