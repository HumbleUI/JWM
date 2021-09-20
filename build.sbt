import scala.collection.compat.immutable.LazyList
import Dependencies._
import Versions._
import BuildNative._
import scala.sys.process._
import sbt.IO

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

cleanFiles ++= Seq(
  baseDirectory.value / "shared" / "target"
)
val cleanDirs = Seq("build", "target",s"src/main/resources/${BuildNative.getLibName()}")
cleanFiles := cleanFiles
  .dependsOn(windows / cleanFiles, linux / cleanFiles, macos / cleanFiles)
  .value

lazy val cmake = taskKey[Unit]("run cmake")

lazy val ninja = taskKey[Unit]("run ninja")

lazy val copyArtifact = taskKey[Unit]("copy artifact under resources")

version := jwmVersion

// remove scala version suffix
ThisBuild / crossPaths := false
ThisBuild / autoScalaLibrary := false

root / Compile / compile := (root / Compile / compile)
  .dependsOn(cmake, ninja, copyArtifact)
  .value

lazy val root = project
  .in(file("."))
  .settings(
    cmake / baseDirectory := file(
      s"${baseDirectory.value}/${BuildNative.system}"
    ),
    ninja / baseDirectory := file(
      s"${baseDirectory.value}/${BuildNative.system}/build"
    ),
    copyArtifact / baseDirectory := file(s"${baseDirectory.value}"),
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
      val log = streams.value.log
      println("Running ninja 🥷🏻")
      Process(
        "ninja",
        (ninja / baseDirectory).value
      ) ! log
    },
    ninja := ninja.dependsOn(cmake).value,
    copyArtifact := {
      println("copying artifact under resource directory")
      val arch = BuildNative.arch
      val system = BuildNative.system
      val buildDir = file(s"${(copyArtifact / baseDirectory).value}/$system/build")
      val resDir =
        file(s"${(copyArtifact / baseDirectory).value}/$system/src/main/resources")
      IO.copyFile(buildDir/ getLibName() ,resDir/ getLibName())
    },
    copyArtifact := copyArtifact.dependsOn(ninja).value
  )
  .aggregate(shared, windows, linux, macos)

lazy val shared = project
  .in(file("shared"))
  .settings(
    name := "jwm-shared",
    libraryDependencies ++= Dependencies.shared
  )

lazy val windows = project
  .in(file("windows"))
  .settings(
    cleanFiles ++= cleanDirs.map(dir => baseDirectory.value / dir),
    name := "jwm-windows-x64",
    libraryDependencies ++= Dependencies.jwmShared
  )
  .dependsOn(shared)
lazy val linux = project
  .in(file("linux"))
  .settings(
    cleanFiles ++= cleanDirs.map(dir => baseDirectory.value / dir),
    name := "jwm-linux-x64",
    libraryDependencies ++= Dependencies.jwmShared
  )
  .dependsOn(shared)
lazy val macos = project
  .in(file("macos"))
  .settings(
    cleanFiles ++= cleanDirs.map(dir => baseDirectory.value / dir),
    libraryDependencies ++= Dependencies.jwmShared
  )
  .dependsOn(shared)
