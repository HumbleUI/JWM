import java.nio.file.Path
import scala.sys.process._
import sbt._
object Deploy {

  def revision():String = {
    val withSuffix = "([0-9]+.[0-9]+).0-([0-9]+)-[a-z0-9]+".r
    val noSuffix = "([0-9]+.[0-9]+).0".r
    describe() match {
      case Some(withSuffix(majorMinor,meta)) => s"$majorMinor.$meta"
      case Some(noSuffix(majorMinor)) => s"$majorMinor.0"
    }
  }

  private def describe():Option[String] = {
    val os   = new java.io.ByteArrayOutputStream

    val code = Seq(
        "git", "describe", "--tags", "--match", "*.*.0"
      ) #> os !
    
    os.close()
    
    if(code == 0){
        Some(os.toString("UTF-8").stripLineEnd)
    }else None
  }
}
