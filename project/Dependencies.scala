import sbt._
import Deploy._
object Dependencies {
  val shared = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20"
  )
  def jwmShared(version:String) = Seq(
    "org.jetbrains.jwm" % "jwm-shared" % version
  )
}
