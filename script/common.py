#! /usr/bin/env python3
import argparse, os, pathlib, platform, subprocess, sys, urllib.request

arch = {'AMD64': 'x64', 'x86_64': 'x64', 'arm64': 'arm64'}[platform.machine()]
parser = argparse.ArgumentParser()
parser.add_argument('--arch', default=arch)
(args, _) = parser.parse_known_args()
arch = args.arch
system = {'Darwin': 'macos', 'Linux': 'linux', 'Windows': 'windows'}[platform.system()]
classpath_separator = ';' if system == 'windows' else ':'
mvn = "mvn.cmd" if system == "windows" else "mvn"
space_skija = 'https://packages.jetbrains.team/maven/p/skija/maven'
space_jwm = 'https://packages.jetbrains.team/maven/p/jwm/maven'

def fetch(url, file):
  if not os.path.exists(file):
    print('Downloading', url)
    if os.path.dirname(file):
      os.makedirs(os.path.dirname(file), exist_ok = True)
    with open(file, 'wb') as f:
      f.write(urllib.request.urlopen(url).read())

def fetch_maven(group, name, version, classifier=None, repo='https://repo1.maven.org/maven2'):
  path = '/'.join([group.replace('.', '/'), name, version, name + '-' + version + ('-' + classifier if classifier else '') + '.jar'])
  file = os.path.join(os.path.expanduser('~'), '.m2', 'repository', path)
  fetch(repo + '/' + path, file)
  return file

def deps():
  return [
    fetch_maven('org.projectlombok', 'lombok', '1.18.20'),
    fetch_maven('org.jetbrains', 'annotations', '20.1.0'),
  ]

def javac(classpath, sources, target, release='11', opts=[]):
  classes = {path.stem: path.stat().st_mtime for path in pathlib.Path(target).rglob('*.class') if '$' not in path.stem}
  newer = lambda path: path.stem not in classes or path.stat().st_mtime > classes.get(path.stem)
  new_sources = [path for path in sources if newer(pathlib.Path(path))]
  if new_sources:
    print('Compiling', len(new_sources), 'java files to', target + ':', new_sources)
    subprocess.check_call([
      'javac',
      '-encoding', 'UTF8'] + opts + [
      '--release', release,
      '--class-path', classpath_separator.join(classpath + [target]),
      '-d', target] + new_sources)
