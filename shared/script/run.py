#! /usr/bin/env python3
import argparse, common, glob, os, platform, subprocess, sys
sys.path.append(os.path.normpath(os.path.dirname(__file__) + '/../../' + common.system))
import script.build as native_build
import build as shared_build

def main():
  shared_build.main()
  native_build.main()

  parser = argparse.ArgumentParser()
  parser.add_argument('example', nargs='?', default='SingleWindow')
  args = parser.parse_args()

  os.chdir(os.path.dirname(__file__) + '/..')
  compile_classpath = [
    common.fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    common.fetch_maven('org.jetbrains', 'annotations', '19.0.0')
  ]
  sources = ['examples/' + args.example + '.java']
  common.javac(compile_classpath, sources, 'target/classes')

  run_classpath = [
    'target/classes',
    '../' + common.system + '/build'
  ]
  subprocess.check_call([
    'java',
    '--class-path', common.classpath_separator.join(run_classpath)]
    + (['-XstartOnFirstThread'] if 'macos' == common.system else [])
    + ['-Djava.awt.headless=true',
    '-enableassertions',
    '-enablesystemassertions',
    '-Xcheck:jni',
    'org.jetbrains.jwm.examples.' + args.example
  ])

  return 0

if __name__ == '__main__':
  sys.exit(main())
