import sbt._
import Utils._
object Dependencies {
  val skijaVersion = "0.93.1"
  val jwmVersion = "0.0.0-SNAPSHOT"
  val deps = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20",
    "io.github.humbleui.skija" % "skija-shared" % skijaVersion,
    "io.github.humbleui.jwm" % "jwm" % jwmVersion
  ) ++ platformDeps

  private def platformDeps = Utils.system match {
    case OS.Windows =>
      Seq("io.github.humbleui.skija" % "skija-windows" % skijaVersion)
    case OS.Linux =>
      Seq("io.github.humbleui.skija" % "skija-linux" % skijaVersion)
    case OS.Mac =>
      Seq(
        "io.github.humbleui.skija" % "skija-macos-x64" % skijaVersion,
        "io.github.humbleui.skija" % "skija-macos-arm64" % skijaVersion
      )
  }
}

