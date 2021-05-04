#! /usr/bin/env python3
import common, glob, os, platform, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')

  # shared
  classpath = [
    common.fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    common.fetch_maven('org.jetbrains', 'annotations', '19.0.0')
  ]
  sources = glob.glob('shared/java/*.java') + glob.glob('shared/java/impl/*.java')
  common.javac(classpath, sources, 'shared/target/classes')

  # platform
  subprocess.check_call(["cmake", "-B", "build", "-G", "Ninja"], cwd = common.system)
  subprocess.check_call(["ninja"], cwd = common.system + "/build")

  classpath = [
    common.fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    common.fetch_maven('org.jetbrains', 'annotations', '19.0.0'),
    'shared/target/classes'
  ]
  sources = glob.glob(common.system + '/java/*.java')
  common.javac(classpath, sources, common.system + '/target/classes')

  return 0

if __name__ == '__main__':
  sys.exit(main())
