#! /usr/bin/env python3
import argparse, common, glob, os, re, subprocess, sys
from typing import List, Tuple
from contextlib import contextmanager 

def parse_version(args: object) -> str:
  """
  Parse version from commandline arguments. Arguments should contain --ref or --version
  
  If both ref and version are provided, this function uses ref.

  Args:
    args (object): Argparser argument object
  Returns:
    version (str)
  """
  if hasattr(args, 'ref') and args.ref is not None:
    return parse_refs(args)
  if hasattr(args, 'version') and args.version is not None:
    return args.version
  raise ValueError("""
    --ref or --version argument should be provided.\n
    Examples:
      python package.py --ref refs/tag/x.y.z\n
      python package.py --version x.y.z
    """)

def parse_refs(args: object) -> str:
  return re.fullmatch("refs/[^/]+/([^/]+)", args.ref).group(1)

@contextmanager
def use_pom(
  rev: str="0.0.0-SNAPSHOT",
  pom_src: str = os.path.join(common.basedir, "deploy/META-INF/maven/io.github.humbleui.jwm/jwm/pom.xml")) -> None:
  """
  return versioned pom in block.

  Args:
    rev (str): app version. default is 0.0.0-SNAPSHOT
    pom_src (str): pom.xml source path. Absolute path or relative directory from common.basedir 
  
  Returns:
    None
  
  Example:
  with use_pom(rev) as pom:
    with open(path/to/target) as f:
      f.write(pom)
  """
  with open(pom_src,"r") as f:
    pom = f.read()
    pom = pom.replace("${version}", rev)
    try:
      yield pom
    finally:
      pass

def package_jar(
  outjar: str,
  classes: str = os.path.join(common.basedir,"target/classes"),
  jarCmdArgs: List[str] = []
  ) -> str:
  """
  Args:
    outjar (str): absolute path to output jar file.
    classes (str): path to source classes directory.
    jarCmdArgs (List[str]): additional arguments passed to `jar --create` command.
  Returns:
    outjar (str): path to output jar file.
  """
  assert os.path.isabs(outjar), "outjar must be absolute path"
  print(f"Packaging {os.path.basename(outjar)}")
  targetmaven = os.path.join(common.basedir,"target/maven")
  subprocess.check_call(["jar",
    "--create",
    "--file", outjar,
    "-C", classes, ".",
    "-C", targetmaven, "META-INF"]
    + (["-C", common.target_native_dir, common.target_native_lib] if os.path.exists(common.target_native_dir + "/" + common.target_native_lib) else [])
    + jarCmdArgs)
  return outjar

def package(rev: str, jarCmdArgs: List[str] = []) -> Tuple[str,str,str]:
  """
  Args:
    rev (str): Version number for jar, sourcejar, javadoc and pom.
    jarCmdArgs (List[str]): additional arguments passed to `jar --create` command.
  
  Returns:
    outjarpath, sourcejar_path, javadoc_path(Tuple[str, str, str]): Absolute paths to generated files. 
  """
  # Update poms
  jwmtarget = os.path.join(common.basedir, "target/maven/META-INF/maven/io.github.humbleui.jwm/jwm")
  with use_pom(rev) as pom:
    os.makedirs(jwmtarget, exist_ok=True)
    with open(os.path.join(jwmtarget,"pom.xml"), "w") as f:
      f.write(pom)

  with open("deploy/META-INF/maven/io.github.humbleui.jwm/jwm/pom.properties", "r") as f:
    contents = f.read()
    with open("target/maven/META-INF/maven/io.github.humbleui.jwm/jwm/pom.properties", "w") as f:
      f.write(contents.replace("${version}", rev))
  
  targetclasses =os.path.join(common.basedir,"target/classes")
  os.makedirs(targetclasses, exist_ok = True)

  with open(os.path.join(targetclasses,"jwm.version"), 'w') as f:
    f.write(rev)

  # jwm-*.jar
  outjar = package_jar(os.path.join(common.basedir,f"target/jwm-{rev}.jar"), targetclasses, jarCmdArgs = jarCmdArgs)

  # jwm-*-sources.jar
  print(f"Packaging jwm-{rev}-sources.jar")
  
  lombok = common.deps()[0]
  subprocess.check_call(["java",
    "-jar", lombok,
    "delombok",
    *common.javasources,
    "--classpath", common.classpath_separator.join(common.deps()),
    "-d", "target/delomboked/io/github/humbleui/jwm"
  ])
  sourcejar = os.path.join(common.basedir,f"target/jwm-{rev}-sources.jar")
  subprocess.check_call(["jar",
    "--create",
    "--file", sourcejar,
    "-C", "target/delomboked", ".",
    "-C", "target/maven", "META-INF"
  ])

  # jwm-*-javadoc.jar
  print('Packaging jwm-' + rev + "-javadoc.jar")

  sources = glob.glob("target/delomboked/**/*.java", recursive=True)
  subprocess.check_call(["javadoc",
    "--class-path", common.classpath_separator.join(common.deps()),
    "-d", "docs/apidocs",
    "-quiet",
    "-Xdoclint:all,-missing"]
    + sources)
  javadoc = os.path.join(common.basedir,f"target/jwm-{rev}-javadoc.jar")
  subprocess.check_call(["jar",
    "--create",
    "--file", javadoc,
    "-C", "docs/apidocs", ".",
  ])

  return (outjar, sourcejar, javadoc)

def main() -> int:
  os.chdir(common.basedir)
  parser = argparse.ArgumentParser()
  parser.add_argument('--ref')
  parser.add_argument('--version')
  
  (args, _) = parser.parse_known_args()
  version = parse_version(args)
  package(version)
  return 0

if __name__ == "__main__":
  sys.exit(main())
