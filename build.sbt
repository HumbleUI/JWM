import scala.collection.compat.immutable.LazyList
import Dependencies._
import BuildNative._
import scala.sys.process._
import sbt.IO

// general
name := "jwm"
ThisBuild / organization := "org.jetbrains.jwm"
Compile / javacOptions ++= Seq(
  "-source",
  "11",
  "-target",
  "11",
  "-encoding",
  "utf8"
)

// release stuffs
publishMavenStyle := true
Test / publishArtifact := false
Compile / packageDoc / publishArtifact := false
publishTo := Some(
  "space maven" at "https://packages.jetbrains.team/maven/p/jwm/maven"
)

// remove scala version suffix
ThisBuild / crossPaths := false
ThisBuild / autoScalaLibrary := false
makePom / publishArtifact := true

cleanFiles ++= Seq(
  baseDirectory.value / "shared" / "target"
)
val cleanDirs =
  Seq("build", "target", s"src/main/resources/${BuildNative.getLibName()}")
cleanFiles := cleanFiles
  .dependsOn(windows / cleanFiles, linux / cleanFiles, macos / cleanFiles)
  .value
lazy val cleanSettings = Def.settings(
  cleanFiles ++= cleanDirs.map(dir => baseDirectory.value / dir)
)

lazy val cmake = taskKey[Unit]("run cmake")

lazy val ninja = taskKey[sbt.File]("run ninja")

lazy val packArtifact =
  taskKey[sbt.File]("save native artifact under resources dir")

lazy val commonSettings = Seq(
  libraryDependencies ++= Dependencies.jwmShared(version.value)
)

def publishSettihgs(system: String) = Def.settings(
  publishLocal := publishLocal.dependsOn((shared / publishLocal)).value,
  publishLocal / skip := !name.value.contains(system),
  publish := publish.dependsOn((shared / publish)).value,
  publish / skip := !name.value.contains(system),
  publishM2 := publish.dependsOn((shared / publish)).value,
  publishM2 / skip := !name.value.contains(system)
)

lazy val root = project
  .in(file("."))
  .settings(
    cmake / baseDirectory := file(
      s"${baseDirectory.value}/${BuildNative.system}"
    ),
    cmake := {
      val arch = BuildNative.arch
      val osxArch = BuildNative.osxArch
      val log = streams.value.log
      println("Building native")
      println("Running cmake")
      Process(
        Seq(
          "cmake",
          "-DCMAKE_BUILD_TYPE=Release",
          "-B",
          "build",
          "-G",
          "Ninja",
          s"-DJWM_ARCH=$arch",
          osxArch.getOrElse("")
        ),
        (cmake / baseDirectory).value
      ) ! log
    },
    ninja := {
      cmake.value
      val log = streams.value.log
      println("Running ninja 🥷🏻")
      val outDir = (cmake / baseDirectory).value / "build"
      Process(
        "ninja",
        outDir
      ) ! log
      val out = outDir / getLibName()
      println(s"Build $out")
      out
    },
    packArtifact := {
      println("copying artifact under resource directory")
      val arch = BuildNative.arch
      val system = BuildNative.system
      val resDir = baseDirectory.value / system / "src/main/resources"
      IO.copyFile(ninja.value, resDir / getLibName())
      resDir / getLibName()
    },
    publish / skip := true
  )
  .aggregate(shared,windows, linux, macos)
lazy val shared = project
  .in(file("shared"))
  .settings(
    name := "jwm-shared",
    libraryDependencies ++= Dependencies.shared
  )

lazy val windows = project
  .in(file("windows"))
  .settings(
    name := s"jwm-${name.value}-$arch"
  )
  .settings(cleanSettings)
  .settings(commonSettings)
  .settings(publishSettihgs(system))
  .dependsOn(shared)

lazy val linux = project
  .in(file("linux"))
  .settings(
    name := s"jwm-linux-$arch"
  )
  .settings(cleanSettings)
  .settings(commonSettings)
  .settings(publishSettihgs(system))
  .dependsOn(shared)

lazy val macos = project
  .in(file("macos"))
  .settings(
    name := s"jwm-macos-$arch"
  )
  .settings(commonSettings)
  .settings(cleanSettings)
  .settings(publishSettihgs(s"$system-$arch"))
  .dependsOn(shared)
