#! /usr/bin/env python3
import argparse, common, glob, os, platform, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')

  parser = argparse.ArgumentParser()
  parser.add_argument('example', nargs='?', default='Example')
  args = parser.parse_args()

  space = 'https://packages.jetbrains.team/maven/p/skija/maven'
  skija_native = 'skija-' + common.system + (('-' + common.arch) if 'macos' == common.system else '')
  compile_classpath = [
    common.fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    common.fetch_maven('org.jetbrains', 'annotations', '19.0.0'),
    'shared/target/classes',
    common.system + '/target/classes',
    common.fetch_maven('org.jetbrains.skija', 'skija-shared', '0.91.3', repo=space),
    common.fetch_maven('org.jetbrains.skija', skija_native, '0.91.3', repo=space),
    # '/Users/prokopov/ws/skija/native/build',
    # '/Users/prokopov/ws/skija/shared/target/classes',
  ]
  sources = glob.glob('examples/*.java')
  common.javac(compile_classpath, sources, 'examples/target/classes')

  run_classpath = [
    'shared/target/classes',
    common.system + '/build',
    common.system + '/target/classes',
    'examples/target/classes',
    common.fetch_maven('org.jetbrains.skija', 'skija-shared', '0.91.3', repo=space),
    common.fetch_maven('org.jetbrains.skija', skija_native, '0.91.3', repo=space),
    # '/Users/prokopov/ws/skija/native/build',
    # '/Users/prokopov/ws/skija/shared/target/classes',
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
