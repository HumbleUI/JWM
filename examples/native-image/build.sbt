import sbt._
import com.typesafe.sbt.packager.windows._
import Pack._
import Utils._
import Dependencies._
import scala.sys.process._

resolvers += "jetbrain skija" at "https://packages.jetbrains.team/maven/p/skija/maven"
val mavenLocal = "Local Maven Repository" at "file://"+Path.userHome+"/.m2/repository"
resolvers += mavenLocal
// general
name := "example"
version := "0.0.0"
ThisBuild / organization := "io.github.humbleui.jwm.examples.jwm"

// package stuff
maintainer := "io.github.HumbleUI"
packageSummary := "example jwm application"
packageDescription := """example jwm application."""
enablePlugins(JavaAppPackaging, WindowsPlugin)

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
      Pack.packageLinuxAppCmd(
        name.value,
        organization.value,
        version.value,
        None,
        false,
        baseDirectory.value
      )
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
// use sources from sibling directory
lazy val srcSettings = Seq(
    Compile / javaSource := baseDirectory.value / ".." / "dashboard" / "java",
    Compile / resourceDirectory := baseDirectory.value / ".." / "dashboard" / "resources"
)


lazy val example = project
  .in(file("."))
  .settings(srcSettings)
  .settings(
     libraryDependencies ++= Dependencies.deps,
    assembly / mainClass := Some("io.github.humbleui.jwm.examples.Example"),
    Compile / mainClass := Some("io.github.humbleui.jwm.examples.Example"),
    nativeImageJvmIndex := "jabba",
    nativeImageVersion := "21.1.0",
    nativeImageJvm := "graalvm-ce-java16",
    nativeImageOptions +=s"-H:ReflectionConfigurationFiles=${target.value / "native-image-configs" / "reflect-config.json"}",
    nativeImageOptions += s"-H:ConfigurationFileDirectories=${target.value / "native-image-configs"}",
    nativeImageOptions +="-H:+JNI",
    nativeImageOptions +="--no-fallback",
    nativeImageOptions +="--report-unsupported-elements-at-runtime",
    nativeImageOptions +="--allow-incomplete-classpath",
    assembly / assemblyMergeStrategy := {
      case PathList(ps @ _*) if ps.last endsWith "module-info.class" =>
        MergeStrategy.discard
      case other =>
        MergeStrategy.defaultMergeStrategy(other)
    },
    fork := system == OS.Mac,
    javaOptions ++= {
      system match {
        case OS.Mac => Seq("-XstartOnFirstThread")
        case _ => Seq()
      }
    }
  ).enablePlugins(NativeImagePlugin)

