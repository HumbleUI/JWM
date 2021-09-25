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
    "-C", "macos/target/classes", ".",
    "-C", "shared/target/classes", ".",
    "-C", "examples/target/classes", ".",
    "-C", "/Users/prokopov/ws/skija/shared/target/classes", ".",])
  shutil.copy('macos/build/libjwm_x64.dylib', 'target')
  shutil.copy('/Users/prokopov/ws/skija/native/build/libskija_x64.dylib', 'target')

  os.chdir('target')
  # https://github.com/cubuspl42/JavaFX-Graal-HelloWorld
  # https://github.com/maxum2610/HelloJFX-GraalSVM
  subprocess.check_call([
    '/Library/Java/JavaVirtualMachines/graalvm-ce-java16-21.1.0/Contents/Home/bin/native-image',
    '--server-shutdown-all',
    '--no-fallback',
    '--report-unsupported-elements-at-runtime',
    '--enable-all-security-services',
    '--allow-incomplete-classpath',
    '-H:+JNI',
    '-jar', 'jwm.jar'
  ])

  return 0

if __name__ == '__main__':
  sys.exit(main())
