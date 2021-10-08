import sbt._
import Utils._
object Dependencies {
  val skijaVersion = "0.93.1"
  // If you want to use local snapshot version, 
  // run `./script/publish_local.py --version <jwmVersion>` at jwm directory in advance
   val jwmVersion = "0.0.0-SNAPSHOT"
  //val jwmversion = "0.2.4"
  val deps = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20",
    "org.jetbrains.skija" % "skija-shared" % skijaVersion,
    "io.github.humbleui.jwm" % "jwm" % jwmVersion
  ) ++ platformDeps

  private def platformDeps = Utils.system match {
    case OS.Windows =>
      Seq("org.jetbrains.skija" % "skija-windows" % skijaVersion)
    case OS.Linux =>
      Seq("org.jetbrains.skija" % "skija-linux" % skijaVersion)
    case OS.Mac =>
      Seq(
        "org.jetbrains.skija" % "skija-macos-x64" % skijaVersion,
        "org.jetbrains.skija" % "skija-macos-arm64" % skijaVersion
      )
  }
}

