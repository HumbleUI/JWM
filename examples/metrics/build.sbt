import sbt._
import Dependencies._
import scala.sys.process._
resolvers += "jetbrain skija" at "https://packages.jetbrains.team/maven/p/skija/maven"

lazy val genIconIfNotExists = taskKey[Unit]("generate icon for title bar")

genIconIfNotExists := {
  val macIcon = (Compile / resourceDirectory).value / "macos.icns" 
  val winIcon = (Compile / resourceDirectory).value / "windows.ico"
  System.getProperty("os.name").toLowerCase match {
    case mac if mac.contains("mac") && !macIcon.exists() => {
      Process(
        Seq(
          "iconutil",
          "-c",
          "icns",
          "macos.iconset"
        ),
        ( Compile / resourceDirectory).value
      )!
    }
    case win if win.contains("win") && !winIcon.exists() => {
      Process(
        "convert" +: Seq(16, 24, 32, 48, 256)
          .map(dim => s"windows/icon_${dim}x${dim}.png") :+ "windows.ico",
          (Compile / resourceDirectory).value
      )!
    }
    case _ => ()
  }
}

Compile / run := (Compile / run).dependsOn(genIconIfNotExists).evaluated

ThisBuild / organization := "org.jetbrains.jwm"
javaOptions ++= Seq(
  "enablesystemassertions",
  "-enableassertions",
  "-Xcheck:jni",
  "-Dfile.encoding=UTF-8",
  "-release","16"
)
name := "metrics"
version := "0.1.0-SNAPSHOT"

ThisBuild / crossPaths := false
ThisBuild / autoScalaLibrary := false
lazy val metrics = project
  .in(file("."))
  .settings(
    libraryDependencies ++= Dependencies.deps,
    assembly / mainClass := Some("org.jetbrains.jwm.examples.Example"),
    assembly / assemblyMergeStrategy  := {
      case PathList(ps @ _*) if ps.last endsWith "module-info.class" => MergeStrategy.discard
      case other =>
        MergeStrategy.defaultMergeStrategy(other)
    }
  )
