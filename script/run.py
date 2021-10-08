#! /usr/bin/env python3
import argparse, build, common, glob, os, platform, subprocess, sys

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('example', nargs='?', default='Example')
  parser.add_argument('--jwm-version', default=None)
  parser.add_argument('--skija-version', default='0.92.22')
  parser.add_argument('--skija-dir', default=None)
  args = parser.parse_args()

  if not args.jwm_version:
    build.build_native()
    build.build_java()

  if args.skija_dir:
    skija_dir = os.path.abspath(args.skija_dir)
  
  os.chdir(os.path.dirname(__file__) + '/..')

  # compile
  compile_classpath = common.deps()
  if args.jwm_version:
    compile_classpath += [
      common.fetch_maven('io.github.humbleui.jwm', 'jwm', args.jwm_version)
    ]
  else:
    compile_classpath += [
      'target/classes',
      common.system + '/build'
    ]
  if args.skija_dir:
    compile_classpath += [
      skija_dir + '/platform/build',
      skija_dir + '/platform/target/classes',
      skija_dir + '/shared/target/classes',
    ]
  else:
    skija_native = 'skija-' + common.system + (('-' + common.arch) if 'macos' == common.system else '')
    compile_classpath += [
      common.fetch_maven('org.jetbrains.skija', 'skija-shared', args.skija_version, repo=common.space_skija),
      common.fetch_maven('org.jetbrains.skija', skija_native, args.skija_version, repo=common.space_skija),
    ]
  sources = glob.glob('examples/dashboard/java/**/*.java', recursive=True)
  common.javac(compile_classpath, sources, 'examples/dashboard/target/classes', release='16')
  print(compile_classpath)
  # run
  run_classpath = compile_classpath + ['examples/dashboard/target/classes']
  subprocess.check_call([
    'java',
    '--class-path', common.classpath_separator.join(run_classpath)]
    + (['-XstartOnFirstThread'] if 'macos' == common.system else [])
    + ['-Djava.awt.headless=true',
    '-enableassertions',
    '-enablesystemassertions',
    '-Dfile.encoding=UTF-8',
    '-Xcheck:jni',
    'io.github.humbleui.jwm.examples.' + args.example
  ])

  return 0

if __name__ == '__main__':
  sys.exit(main())
