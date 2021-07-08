#! /usr/bin/env python3
import argparse, build, common, glob, os, platform, subprocess, sys

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('example', nargs='?', default='Example')
  parser.add_argument('--jwm-version', default=None)
  parser.add_argument('--skija-version', default='0.91.4')
  parser.add_argument('--skija-dir', default=None)
  args = parser.parse_args()

  if not args.jwm_version:
    build.main()

  if args.skija_dir:
    skija_dir = os.path.abspath(args.skija_dir)
  
  os.chdir(os.path.dirname(__file__) + '/..')

  # compile
  compile_classpath = common.deps()
  if args.jwm_version:
    compile_classpath += [
      common.fetch_maven('org.jetbrains.jwm', 'jwm-shared', args.jwm_version, repo=common.space_jwm),
      common.fetch_maven('org.jetbrains.jwm', 'jwm-' + common.system + '-' + common.arch, args.jwm_version, repo=common.space_jwm),
    ]
  else:
    compile_classpath += [
      'shared/target/classes',
      common.system + '/build',
      common.system + '/target/classes',
    ]
  if args.skija_dir:
    compile_classpath += [
      skija_dir + '/native/build',
      skija_dir + '/shared/target/classes',
    ]
  else:
    skija_native = 'skija-' + common.system + (('-' + common.arch) if 'macos' == common.system else '')
    compile_classpath += [
      common.fetch_maven('org.jetbrains.skija', 'skija-shared', args.skija_version, repo=common.space_skija),
      common.fetch_maven('org.jetbrains.skija', skija_native, args.skija_version, repo=common.space_skija),
    ]
  sources = glob.glob('examples/java/org/jetbrains/jwm/examples/*.java') + glob.glob('examples/java/org/jetbrains/jwm/examples/' + common.system + '/*.java')
  common.javac(compile_classpath, sources, 'examples/target/classes')

  # run
  run_classpath = compile_classpath + ['examples/target/classes']
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
