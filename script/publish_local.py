#! /usr/bin/env python3
import argparse, common,os, sys, shutil,package
from package import use_pom

def publish_local(rev: str) -> None:
  """
  publish jar to local maven repository.

  Args:
    rev (str): Version number for jar, sourcejar, javadoc and pom.   
  """

  (outjar, sourcejar, javadocs) = package.package(rev)

  local_repo = os.path.expanduser(f"~/.m2/repository/io/github/humbleui/jwm/jwm/{rev}")
  os.makedirs(local_repo, exist_ok=True)
  
  with use_pom(rev) as pom:
    with open(os.path.join(local_repo, f"jwm-{rev}.pom"), "w") as f:
      f.write(pom)

  print(f"Publishing jwm-{rev} to {local_repo}")
  
  # copy jar, sourcejar and javadoc at local maven repo 
  shutil.copy(outjar, os.path.join(local_repo, f"jwm-{rev}.jar"))
  shutil.copy(sourcejar, os.path.join(local_repo, f"jwm-{rev}-sources.jar"))
  shutil.copy(javadocs, os.path.join(local_repo, f"jwm-{rev}-javadoc.jar"))

def main() -> int:
  os.chdir(common.basedir)
  parser = argparse.ArgumentParser()
  parser.add_argument('--version')
  (args, _) = parser.parse_known_args()
  
  version = args.version or "0.0.0-SNAPSHOT"
  
  publish_local(version)
  return 0

if __name__ == "__main__":
  sys.exit(main())
