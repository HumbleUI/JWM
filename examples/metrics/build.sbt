import sbt._
import com.typesafe.sbt.packager.windows._
import Pack._
import Utils._
import Dependencies._
import scala.sys.process._

resolvers += "jetbrain skija" at "https://packages.jetbrains.team/maven/p/skija/maven"

// general
name := "metrics"
version := "0.0.0"
ThisBuild / organization := "org.jetbrains.jwm"
javaOptions ++= Seq(
  "enablesystemassertions",
  "-enableassertions",
  "-Xcheck:jni",
  "-Dfile.encoding=UTF-8",
  "-release",
  "16"
)

// package stuff
maintainer := "john doe"
packageSummary := "example jwm application"
packageDescription := """example jwm application."""

enablePlugins(JavaAppPackaging /*WindowsPlugin*/)

wixProductId := java.util.UUID.randomUUID().toString
wixProductUpgradeId := java.util.UUID.randomUUID().toString

ThisBuild / crossPaths := false
ThisBuild / autoScalaLibrary := false

Compile / run := (Compile / run).dependsOn(genIconIfNotExists).evaluated

lazy val genIconIfNotExists = taskKey[Unit]("generate icon for title bar")

lazy val packageApp = taskKey[Unit]("package application via jpackage")

packageApp := {
  system match {
    case OS.Linux =>
      (Debian / packageBin).value
    case OS.Mac => ???
    case OS.Windows =>
      Def.task {
        println("use windows/packageBin from sbt-native-packager instead.")
        (Windows / packageBin).value
      }.value
  }
  println(s"save package installer at ${baseDirectory.value}/target/${system.name}")
}

genIconIfNotExists := {
  val macIcon = (Compile / resourceDirectory).value / "macos.icns"
  val winIcon = (Compile / resourceDirectory).value / "windows.ico"
  system match {
    case OS.Mac if !macIcon.exists() => {
      Process(
        Seq(
          "iconutil",
          "-c",
          "icns",
          "macos.iconset"
        ),
        (Compile / resourceDirectory).value
      ) !
    }
    case OS.Windows if !winIcon.exists() => {
      Process(
        "convert" +: Seq(16, 24, 32, 48, 256)
          .map(dim => s"windows/icon_${dim}x${dim}.png") :+ "windows.ico",
        (Compile / resourceDirectory).value
      ) !
    }
    case _ => ()
  }
}

lazy val metrics = project
  .in(file("."))
  .settings(
    libraryDependencies ++= Dependencies.deps,
    assembly / mainClass := Some("org.jetbrains.jwm.examples.Example"),
    assembly / assemblyMergeStrategy := {
      case PathList(ps @ _*) if ps.last endsWith "module-info.class" =>
        MergeStrategy.discard
      case other =>
        MergeStrategy.defaultMergeStrategy(other)
    }
  )
