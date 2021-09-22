import sbt._
import Utils._
object Dependencies {
  val skijaVersion = "0.93.1"
  val jwmVersion = "0.1.285"
  val deps = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20",
    "org.jetbrains.skija" % "skija-shared" % skijaVersion,
    "org.jetbrains.jwm" % s"jwm-${system.name}-x64" % jwmVersion
  ) ++ platformDeps

  private def platformDeps = System.getProperty("os.name").toLowerCase match {
    case win if win.contains("win") =>
      Seq("org.jetbrains.skija" % "skija-windows" % skijaVersion)
    case linux if linux.contains("linux") =>
      Seq("org.jetbrains.skija" % "skija-linux" % skijaVersion)
    case macos if macos.contains("mac") =>
      Seq(
        "org.jetbrains.skija" % "skija-macos-x64" % skijaVersion,
        "org.jetbrains.skija" % "skija-macos-arm64" % skijaVersion
      )
  }
}

