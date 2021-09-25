#! /usr/bin/env python3
import argparse, build, clean, common, glob, os, platform, re, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/..')

  parser = argparse.ArgumentParser()
  parser.add_argument('--ref', required=True)
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

  return 0

if __name__ == "__main__":
  sys.exit(main())
