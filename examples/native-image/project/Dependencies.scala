import sbt._
import Utils._
object Dependencies {
  val skijaVersion = "0.98.0"
  val jwmVersion = "0.0.0-SNAPSHOT"
  val deps = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.22",
    "io.github.humbleui" % "skija-shared" % skijaVersion,
    "io.github.humbleui" % "jwm" % jwmVersion
  ) ++ platformDeps

  private def platformDeps = Utils.system match {
    case OS.Windows =>
      Seq("io.github.humbleui" % "skija-windows" % skijaVersion)
    case OS.Linux =>
      Seq("io.github.humbleui" % "skija-linux" % skijaVersion)
    case OS.Mac =>
      Seq(
        "io.github.humbleui" % "skija-macos-x64" % skijaVersion,
        "io.github.humbleui" % "skija-macos-arm64" % skijaVersion
      )
  }
}

