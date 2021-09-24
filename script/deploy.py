#! /usr/bin/env python3
import argparse, build, clean, common, glob, os, platform, re, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  parser = argparse.ArgumentParser()
  parser.add_argument('--ref', required=True)
  (args, _) = parser.parse_known_args()

  print('Deploying jwm-shared-' + rev + ".jar")
  subprocess.check_call([
    common.mvn,
    '--batch-mode',
    '--settings', 'deploy/settings.xml',
    '-Dspace.username=Nikita.Prokopov',
    '-Dspace.password=' + os.getenv('SPACE_TOKEN'),
    'deploy:deploy-file',
    "-Dfile=target/jwm-shared-" + rev + ".jar",
    "-DpomFile=deploy/META-INF/maven/io.github.humbleui.jwm/jwm-shared/pom.xml",
    "-DrepositoryId=space-maven",
    "-Durl=" + common.space_jwm,
  ])

  print('Deploying jwm-shared-' + rev + "-sources.jar")
  subprocess.check_call([
    common.mvn,
    '--batch-mode',
    '--settings', 'deploy/settings.xml',
    '-Dspace.username=Nikita.Prokopov',
    '-Dspace.password=' + os.getenv('SPACE_TOKEN'),
    'deploy:deploy-file',
    "-Dpackaging=java-source",
    "-Dfile=target/jwm-shared-" + rev + "-sources.jar",
    "-DpomFile=deploy/META-INF/maven/io.github.humbleui.jwm/jwm-shared/pom.xml",
    "-DrepositoryId=space-maven",
    "-Durl=" + common.space_jwm,
  ])
