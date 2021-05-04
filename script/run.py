#! /usr/bin/env python3
import argparse, common, glob, os, platform, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')

  parser = argparse.ArgumentParser()
  parser.add_argument('example', nargs='?', default='SingleWindow')
  args = parser.parse_args()

  compile_classpath = [
    common.fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    common.fetch_maven('org.jetbrains', 'annotations', '19.0.0'),
    'shared/target/classes',
    common.system + '/target/classes'
  ]
  sources = ['examples/' + args.example + '.java']
  common.javac(compile_classpath, sources, 'examples/target/classes')

  run_classpath = [
    'shared/target/classes',
    common.system + '/build',
    common.system + '/target/classes',
    'examples/target/classes'
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
