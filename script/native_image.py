#! /usr/bin/env python3
import common, glob, os, platform, shutil, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  os.makedirs('target', exist_ok=True)
  subprocess.check_call(
   ["jar",
    "--create",
    "--file", "target/jwm.jar",
    "--main-class", "io.github.humbleui.jwm.examples.Example",
    "-C", "target/classes", ".",
    # Copy native-image config to JAR:
    # https://www.graalvm.org/reference-manual/native-image/BuildConfiguration/#embedding-a-configuration-file
    "-C", "deploy", "."])
  shutil.copy('windows/build/jwm_x64.dll', 'target')
  #shutil.copy('/Users/prokopov/ws/skija/native/build/libskija_x64.dylib', 'target')

  os.chdir('target')
  # https://github.com/cubuspl42/JavaFX-Graal-HelloWorld
  # https://github.com/maxum2610/HelloJFX-GraalSVM
  subprocess.check_call([
    'C:\\GraalVM\\graalvm-ce-java17-21.3.0-dev\\bin\\native-image.cmd',
    '--no-fallback',
    '--report-unsupported-elements-at-runtime',
    '--allow-incomplete-classpath',
    '-H:+JNI',
    '-jar', 'jwm.jar'
  ])

  return 0

if __name__ == '__main__':
  sys.exit(main())
