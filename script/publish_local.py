#! /usr/bin/env python3
import argparse, build, clean, common, glob, os, platform, re, subprocess, sys, shutil

def main():
  os.chdir(os.path.dirname(__file__) + '/..')

  parser = argparse.ArgumentParser()
  parser.add_argument('--source-jar',action='store_false')
  parser.add_argument('--docs',action='store_false')
  parser.add_argument('--version', required=True)
  
  (args, _) = parser.parse_known_args()

  if "SNAPSHOT" not in args.version:
    print("Warn: Version does not contain SNAPSHOT. Continue? (y/n)")
    if input() not in ["y","Y","yes","Yes"]:
      sys.exit()

  rev = args.version
  # Update poms
  os.makedirs("target/classes", exist_ok=True)
  local_repo = os.path.expanduser(f"~/.m2/repository/io/github/humbleui/jwm/jwm/{rev}")
  os.makedirs(local_repo, exist_ok=True)
  

  with open("deploy/META-INF/maven/io.github.humbleui.jwm/jwm/pom.xml", "r") as f:
    contents = f.read()
    with open(os.path.join(local_repo,f"jwm-{rev}.pom"), "w") as f:
      f.write(contents.replace("${version}", rev))

  with open(os.path.join('target/classes', 'jwm.version'), 'w') as f:
    f.write(rev)

  print(f"publishing jwm-{rev} at {local_repo}")

  # jwm-*.jar
  print('Packaging jwm-' + rev + ".jar")
  subprocess.check_call(["jar",
    "--create",
    "--file", os.path.join(local_repo, f"jwm-{rev}.jar"),
    "-C", "target/classes", ".",
    "-C", "target/maven", "META-INF",
    "-C", f"{common.system}/build", build.target_native 
  ])
  if args.source_jar:
    # jwm-*-sources.jar
    print('Packaging jwm-' + rev + "-sources.jar")

    lombok = common.deps()[0]
    subprocess.check_call(["java",
      "-jar", lombok,
      "delombok",
      "linux/java", "macos/java", "shared/java", "windows/java",
      "--classpath", common.classpath_separator.join(common.deps()),
      "-d", "target/delomboked/io/github/humbleui/jwm"
    ])

    subprocess.check_call(["jar",
      "--create",
      "--file", os.path.join(local_repo,f"jwm-{rev}-sources.jar"),
      "-C", "target/delomboked", ".",
      "-C", "target/maven", "META-INF"
    ])
  if args.docs:
    # jwm-*-javadoc.jar
    print('Packaging jwm-' + rev + "-javadoc.jar")

    sources = glob.glob("target/delomboked/**/*.java", recursive=True)
    subprocess.check_call(["javadoc",
      "--class-path", common.classpath_separator.join(common.deps()),
      "-d", "docs/apidocs",
      "-quiet",
      "-Xdoclint:all,-missing"]
      + sources)

    subprocess.check_call(["jar",
      "--create",
      "--file", os.path.join(local_repo,f"jwm-{rev}-javadoc.jar"),
      "-C", "docs/apidocs", ".",
    ])
  
  print("finish publishing")

  return 0

if __name__ == "__main__":
  sys.exit(main())
