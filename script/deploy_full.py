#! /usr/bin/env python3
import argparse, build, clean, common, glob, os, platform, revision, subprocess, sys

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("--dry-run", action="store_true")
  (args, _) = parser.parse_known_args()


  # Update poms
  os.chdir(os.path.dirname(__file__) + "/../" + "full")
  rev = revision.revision()
  
  with open("deploy/META-INF/maven/org.jetbrains.jwm/jwm-full/pom.xml", "r+") as f:
    pomxml = f.read()
    f.seek(0)
    f.write(pomxml.replace("0.0.0-SNAPSHOT", rev))
    f.truncate()

  with open("deploy/META-INF/maven/org.jetbrains.jwm/jwm-full/pom.properties", "r+") as f:
    pomprops = f.read()
    f.seek(0)
    f.write(pomprops.replace("0.0.0-SNAPSHOT", rev))
    f.truncate()

  # .jar
  print("Packaging " + "jwm-full-" + rev + ".jar")
  subprocess.check_call(["jar",
    "--create",
    "--file", "target/jwm-full-" + rev + ".jar",
    "-C","build","libjwm_x64.dylib",
    "-C","build","libjwm_arm64.dylib",
    "-C", "build", "libjwm_x64.so",
    "-C","build","jwm_x64.dll",
    "-C", "build", "jwm.version",
    "-C", "target/classes", ".",
    "-C", "deploy", "META-INF/maven/org.jetbrains.jwm/jwm-full"
  ])

  if not args.dry_run:
    print("Deploying", "jwm-full-" + rev + ".jar")
    subprocess.check_call([
      common.mvn,
      "--batch-mode",
      "--settings", "deploy/settings.xml",
      "-Dspace.username=Nikita.Prokopov",
      "-Dspace.password=" + os.getenv("SPACE_TOKEN"),
      "deploy:deploy-file",
      "-Dfile=target/jwm-full-" + rev + ".jar",
      "-DpomFile=deploy/META-INF/maven/org.jetbrains.jwm/jwm-full/pom.xml",
      "-DrepositoryId=space-maven",
      "-Durl=" + common.space_jwm,
    ])

  print("Packaging " + "jwm-full-" + rev + "-sources.jar")
  subprocess.check_call([
    "jar",
    "--create",
    "--file", "target/jwm-full-" + rev + "-sources.jar",
    "-C", "target/delomboked", ".",
    "-C", "deploy", "META-INF/maven/org.jetbrains.jwm/jwm-full"
  ])

  if not args.dry_run:
    print("Deploying " + "jwm-full-" + rev + "-sources.jar")
    subprocess.check_call([
      "mvn",
      "--batch-mode",
      "--settings", "deploy/settings.xml",
      "-Dspace.username=Nikita.Prokopov",
      "-Dspace.password=" + os.getenv("SPACE_TOKEN"),
      "deploy:deploy-file",
      "-Dpackaging=java-source",
      "-Dfile=target/jwm-full-" + rev + "-sources.jar",
      "-DpomFile=deploy/META-INF/maven/org.jetbrains.jwm/jwm-full/pom.xml",
      "-DrepositoryId=space-maven",
      "-Durl=" + common.space_jwm,
    ])

  # Restore poms
  with open("deploy/META-INF/maven/org.jetbrains.jwm/jwm-full/pom.xml", "w") as f:
    f.write(pomxml)

  with open("deploy/META-INF/maven/org.jetbrains.jwm/jwm-full/pom.properties", "w") as f:
    f.write(pomprops)

  return 0

if __name__ == "__main__":
  sys.exit(main())
