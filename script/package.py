#! /usr/bin/env python3
import argparse, build, clean, common, glob, os, platform, re, subprocess, sys
import shutil

def main():
  os.chdir(os.path.dirname(__file__) + '/..')

  parser = argparse.ArgumentParser()
  parser.add_argument('--ref', required=True)
  parser.add_argument('--publish-local', action='store_true')
  
  (args, _) = parser.parse_known_args()

  # Update poms
  os.makedirs("target/classes", exist_ok=True)
  os.makedirs("target/maven/META-INF/maven/io.github.humbleui.jwm/jwm", exist_ok=True)

  rev = re.fullmatch("refs/[^/]+/([^/]+)", args.ref).group(1)

  with open("deploy/META-INF/maven/io.github.humbleui.jwm/jwm/pom.xml", "r") as f:
    contents = f.read()
    with open("target/maven/META-INF/maven/io.github.humbleui.jwm/jwm/pom.xml", "w") as f:
      f.write(contents.replace("${version}", rev))

  with open("deploy/META-INF/maven/io.github.humbleui.jwm/jwm/pom.properties", "r") as f:
    contents = f.read()
    with open("target/maven/META-INF/maven/io.github.humbleui.jwm/jwm/pom.properties", "w") as f:
      f.write(contents.replace("${version}", rev))

  with open(os.path.join('target/classes', 'jwm.version'), 'w') as f:
    f.write(rev)

  # jwm-*.jar
  print('Packaging jwm-' + rev + ".jar")
  subprocess.check_call(["jar",
    "--create",
    "--file", "target/jwm-" + rev + ".jar",
    "-C", "target/classes", ".",
    "-C", "target/maven", "META-INF",
  ])

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
    "--file", "target/jwm-" + rev + "-sources.jar",
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

  subprocess.check_call(["jar",
    "--create",
    "--file", "target/jwm-" + rev + "-javadoc.jar",
    "-C", "docs/apidocs", ".",
  ])
  print("finish packaging")

  if args.publish_local == True:
    local_repo = os.path.expanduser(f"~/.m2/repository/io/github/humbleui/jwm/jwm/{rev}")
    print(f"publishing jwm-{rev} at {local_repo}")
    os.makedirs(local_repo, exist_ok=True)
    shutil.copyfile("target/maven/META-INF/maven/io.github.humbleui.jwm/jwm/pom.xml",os.path.join(local_repo,f"jwm-{rev}.pom"))
    shutil.copyfile(f"target/jwm-{rev}.jar",os.path.join(local_repo,  f"jwm-{rev}.jar"))
    shutil.copyfile(f"target/jwm-{rev}-javadoc.jar",os.path.join(local_repo ,  f"jwm-{rev}-javadoc.jar"))
    shutil.copyfile(f"target/jwm-{rev}-sources.jar",os.path.join(local_repo ,f"jwm-{rev}-sources.jar"))
    print(f"finish publishing")

  return 0

if __name__ == "__main__":
  sys.exit(main())
