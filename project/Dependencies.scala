import sbt._
import Versions._
object Dependencies {
  val shared = Seq(
    "org.jetbrains" % "annotations" % "20.1.0",
    "org.projectlombok" % "lombok" % "1.18.20"
  )
  val jwmShared = Seq(
    "org.jetbrains" % "jwm-shared" % jwmVersion 
  )
}
