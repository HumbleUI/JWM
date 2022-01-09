#! /usr/bin/env python3
import argparse, build, build_utils, common, glob, os, platform, subprocess, sys

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--jwm-version', default=None)
  parser.add_argument('--skija-version', default='0.98.0')
  parser.add_argument('--skija-dir', default=None)
  parser.add_argument('--types-dir', default=None)
  args = parser.parse_args()

  if not args.jwm_version:
    build.main()

  if args.skija_dir:
    skija_dir = os.path.abspath(args.skija_dir)
  
  os.chdir(common.basedir)

  # compile
  classpath = common.deps_compile()
  
  if args.jwm_version:
    classpath += [
      build_utils.fetch_maven('io.github.humbleui.jwm', 'jwm', args.jwm_version)
    ]
  else:
    classpath += [
      'target/classes',
      build_utils.system + '/build'
    ]

  if args.skija_dir:
    classpath += [
      skija_dir + '/platform/build',
      skija_dir + '/platform/target/classes',
      skija_dir + '/shared/target/classes',
    ]
  else:
    skija_native = 'skija-' + build_utils.system + (('-' + build_utils.arch) if 'macos' == build_utils.system else '')
    classpath += [
      build_utils.fetch_maven('io.github.humbleui', 'skija-shared', args.skija_version),
      build_utils.fetch_maven('io.github.humbleui', skija_native, args.skija_version),
    ]
  sources = build_utils.files('examples/dashboard/java/**/*.java')
  build_utils.javac(sources, 'examples/dashboard/target/classes', classpath = classpath, release='16')
  
  # run
  subprocess.check_call([
    'java',
    '--class-path', build_utils.classpath_join(classpath + ['examples/dashboard/target/classes']),
    *(['-XstartOnFirstThread'] if 'macos' == build_utils.system else []),
    '-Djava.awt.headless=true',
    '-enableassertions',
    '-enablesystemassertions',
    '-Dfile.encoding=UTF-8',
    '-Xcheck:jni',
    'io.github.humbleui.jwm.examples.Example'
  ])

  return 0

if __name__ == '__main__':
  sys.exit(main())
