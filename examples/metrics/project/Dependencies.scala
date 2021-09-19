import sbt._

object Dependencies {
  val deps = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20",
    "org.jetbrains.skija" % "skija-shared" % "0.93.1",
    "org.jetbrains.skija" % "skija-windows" % "0.93.1",
    "org.jetbrains" % "jwm-windows" % "0.1.0-SNAPSHOT"
  )
}
