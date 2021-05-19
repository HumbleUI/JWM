#! /usr/bin/env python3
import argparse, build, common, glob, os, platform, subprocess, sys

def main():
  build.main()
  
  os.chdir(os.path.dirname(__file__) + '/..')

  parser = argparse.ArgumentParser()
  parser.add_argument('example', nargs='?', default='Example')
  args = parser.parse_args()

  skija_native = 'skija-' + common.system + (('-' + common.arch) if 'macos' == common.system else '')
  compile_classpath = common.deps() + [
    'shared/target/classes',
    common.system + '/target/classes',
    common.fetch_maven('org.jetbrains.skija', 'skija-shared', '0.91.4', repo=common.space),
    common.fetch_maven('org.jetbrains.skija', skija_native, '0.91.4', repo=common.space),
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
    common.fetch_maven('org.jetbrains.skija', 'skija-shared', '0.91.4', repo=common.space),
    common.fetch_maven('org.jetbrains.skija', skija_native, '0.91.4', repo=common.space),
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
