#! /usr/bin/env python3
import argparse, build_utils, common, os, sys, shutil, package

def main() -> int:
  jars = package.main()
  os.chdir(common.basedir)

  repo = os.path.expanduser(f"~/.m2/repository/io/github/humbleui/jwm/{common.version}")
  build_utils.copy_newer("target/maven/META-INF/maven/io.github.humbleui/jwm/pom.xml",
                         f"{repo}/jwm-{common.version}.pom")
  for jar in jars:
    if build_utils.copy_newer(jar, repo + "/" + os.path.basename(jar)):
      print(f"Installed {os.path.basename(jar)}")

  return 0

if __name__ == "__main__":
  sys.exit(main())
