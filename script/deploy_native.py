#! /usr/bin/env python3
import argparse, build, clean, common, glob, os, platform, revision, subprocess, sys

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("--dry-run", action="store_true")
  (args, _) = parser.parse_known_args()

  # Build
  build.build_native()

  # Update poms
  os.chdir(os.path.dirname(__file__) + "/../" + common.system)
  rev = revision.revision()

  artifact = "jwm-" + common.system + "-" + common.arch

  with open("deploy/META-INF/maven/org.jetbrains.jwm/" + artifact + "/pom.xml", "r+") as f:
    pomxml = f.read()
    f.seek(0)
    f.write(pomxml.replace("0.0.0-SNAPSHOT", rev))
    f.truncate()

  with open("deploy/META-INF/maven/org.jetbrains.jwm/" + artifact + "/pom.properties", "r+") as f:
    pomprops = f.read()
    f.seek(0)
    f.write(pomprops.replace("0.0.0-SNAPSHOT", rev))
    f.truncate()

  with open(os.path.join('build', 'jwm.version'), 'w') as f:
    f.write(rev)

  # .jar
  print("Packaging " + artifact + "-" + rev + ".jar")
  subprocess.check_call(["jar",
    "--create",
    "--file", "target/" + artifact + "-" + rev + ".jar",
    "-C", "build", build.target_native,
    "-C", "build", "jwm.version",
    "-C", "target/classes", ".",
    "-C", "deploy", "META-INF/maven/org.jetbrains.jwm/" + artifact
  ])

  if not args.dry_run:
    print("Deploying", artifact + "-" + rev + ".jar")
    subprocess.check_call([
      common.mvn,
      "--batch-mode",
      "--settings", "deploy/settings.xml",
      "-Dspace.username=Nikita.Prokopov",
      "-Dspace.password=" + os.getenv("SPACE_TOKEN"),
      "deploy:deploy-file",
      "-Dfile=target/" + artifact + "-" + rev + ".jar",
      "-DpomFile=deploy/META-INF/maven/org.jetbrains.jwm/" + artifact + "/pom.xml",
      "-DrepositoryId=space-maven",
      "-Durl=" + common.space_jwm,
    ])

  # -sources.jar
  lombok = common.deps()[0]
  print("Delomboking sources")
  classpath = common.deps() + ["../shared/target/classes"]
  subprocess.check_call([
    "java",
    "-jar",
    lombok,
    "delombok",
    "java",
    "--classpath",
    common.classpath_separator.join(classpath),
    "-d", "target/delomboked/org/jetbrains/jwm"
  ])

  print("Packaging " + artifact + "-" + rev + "-sources.jar")
  subprocess.check_call([
    "jar",
    "--create",
    "--file", "target/" + artifact + "-" + rev + "-sources.jar",
    "-C", "target/delomboked", ".",
    "-C", "deploy", "META-INF/maven/org.jetbrains.jwm/" + artifact
  ])

  if not args.dry_run:
    print("Deploying " + artifact + "-" + rev + "-sources.jar")
    mvn = "mvn.cmd" if common.system == "windows" else "mvn"
    subprocess.check_call([
      mvn,
      "--batch-mode",
      "--settings", "deploy/settings.xml",
      "-Dspace.username=Nikita.Prokopov",
      "-Dspace.password=" + os.getenv("SPACE_TOKEN"),
      "deploy:deploy-file",
      "-Dpackaging=java-source",
      "-Dfile=target/" + artifact + "-" + rev + "-sources.jar",
      "-DpomFile=deploy/META-INF/maven/org.jetbrains.jwm/" + artifact + "/pom.xml",
      "-DrepositoryId=space-maven",
      "-Durl=" + common.space_jwm,
    ])

  # Restore poms
  with open("deploy/META-INF/maven/org.jetbrains.jwm/" + artifact + "/pom.xml", "w") as f:
    f.write(pomxml)

  with open("deploy/META-INF/maven/org.jetbrains.jwm/" + artifact + "/pom.properties", "w") as f:
    f.write(pomprops)

  return 0

if __name__ == "__main__":
  sys.exit(main())
