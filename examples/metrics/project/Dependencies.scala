import sbt._

object Dependencies {
  val skijaVersion = "0.93.1"
  val deps = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20",
    "org.jetbrains.skija" % "skija-shared" % skijaVersion,
    "org.jetbrains.skija" % "skija-windows" % skijaVersion,
    "org.jetbrains.jwm" % "jwm-windows-x64" % "0.1.0-SNAPSHOT"
  )
}
