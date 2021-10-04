import sbt._
import scala.sys.process._
import sbt.Keys._
import Utils._
object Pack {
  def packageLinuxAppCmd(
      name: String,
      organization: String,
      version: String,
      appIconPath: Option[String],
      genShortcut: Boolean,
      baseDir: File
  ) = {
    Process(
      Seq(
        "jpackage",
        "--name",
        name,
        "--app-version",
        version,
        "--type",
        "deb",
        "-i",
        "target",
        "--main-class",
        s"${organization}.example.Example",
        // relative path from directory specified by -i option
        "--main-jar",
        s"${name}-assembly-${version}.jar"
      ) ++ appIconPath.map(p => s"--icon $p").toSeq
        ++ { if (genShortcut) Seq("--linux-shortcut") else Seq.empty },
      baseDir
    ).!
  }
}
