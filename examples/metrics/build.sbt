import Dependencies._
resolvers += "jetbrain skija" at "https://packages.jetbrains.team/maven/p/skija/maven"

ThisBuild / organization := "org.jetbrains"

name := "metrics"
version :="0.1.0-SNAPSHOT"

ThisBuild/ crossPaths := false
ThisBuild/ autoScalaLibrary := false
lazy val metrics = project
    .in(file("."))
    .settings(
      libraryDependencies ++= Dependencies.deps
    )
