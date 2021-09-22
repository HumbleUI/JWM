## setup

To setup sbt(simple build tool), install coursier cli and run setup command.

This will automatically install Java, sbt and other useful tools.

linux & macos

Note: If you are M1 mac user, install sbt from github.com/sbt/sbt/releases

```sh
curl -fLo cs https://git.io/coursier-cli-"$(uname | tr LD ld)"
chmod +x cs
./cs install cs
rm cs
cs setup
# this will install Java, sbt(simple build tool)
```

or via homebrew

```sh
brew install coursier/formulas/coursier
cs setup
```

Windows
```sh
bitsadmin /transfer cs-cli https://git.io/coursier-cli-windows-exe "%cd%\cs.exe"
```

For more details, visit https://get-coursier.io/



```sh
# make sure c/c++ build tools exist
clang --version
ninja --version
vim version.sbt
```

```diff
- ThisBuild / version := Deploy.revision()
+ ThisBuild / version := "0.0.0-SNAPSHOT"
```

Now, enter sbt shell and run


```sh
sbt packArtifact publishLocal
# setup jdk 16 for example application
cs java --jvm:openjdk:1.16.0-1 --setup
source ~/.bashrc
cd examples/metrics
vim project/Dependencies.scala
```

```diff
object Dependencies {
  val skijaVersion = "0.93.1"
-  val jwmVersion = "0.1.x"
+  val jwmVersion = "0.0.0-SNAPSHOT"
```

```sh
sbt run
```

## local debug


### clean build

```sh
sbt clean
```

If you want to clean specific project(e.g. windows), run `windows/clean`

You can specify files and directory to be cleaned up in `build.sbt` like this.

```scala
cleanFiles ++= Seq(
  baseDirectory.value / "shared" / "target"
)
```
### publishLocal

```sh
sbt publishLocal
```
This generates package at `~/.ivy2/local/org.jetbrains.jwm/jwm-<os>-<arch>.<ext>`.

```sh
sbt publishM2
```

Similarly, this will publish artifact at user local maven repository(`~/.m2/repository/org/jetbrains/jwm/jwm-<os>-<arch>/version.jar`

sbt will look for these paths when resolving dependencies. Thus, you can debug local artifact by specifying `libraryDependencies += "org.jetbrains.jwm" % "jwm-<os>-<arch>" % <snapshot-version>` in `build.sbt`.

For example, if you want to debug your artifact v `0.0.0-SNAPSHOT` in examples/metrics project,

0. (optional. If cache bothers you, remove `~/.ivy2/local/org.jetbrains.jwm/jwm-<os>-<arch>.<ext>`)
1. run  `sbt publishLocal` at jwm dir.
2. open `examples/metrics/project/Dependencies.scala` and change `jwmVersion` to `0.0.0-SNAPSHOT`.
3. run sbt run at examples/metrics dir.
