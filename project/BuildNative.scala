import sbt._

object BuildNative {

  lazy val arch = getArch()
  lazy val system = getSystem()
  lazy val osxArch = getOSXArch(system,arch)


  def getArch(): String = {
    System.getProperty("os.arch").toLowerCase match {
      case "arm64" => "arm64"
      case "amd64" | "x86_64" => "x64"
    }
  }

  def getLibName():String = system match {
    case "windows" => s"jwm_$arch.dll"
    case "linux" => s"libjwm_$arch.so"
    case "macos" => s"libjwm_$arch.dylib"
  }

  def getSystem():String = {
    System.getProperty("os.name").toLowerCase match {
      case mac if mac.contains("mac") => "macos"
      case win if win.contains("win") => "windows"
      case linux if linux contains("linux") => "linux"
    }
  }

  def getOSXArch(system:String,arch:String) :Option[String] = {
    (system,arch) match {
      case ("macos", "x64") => Some("-DCMAKE_OSX_ARCHITECTURES=x86_64")
      case ("macos","arm64") => Some("-DCMAKE_OSX_ARCHITECTURES=arm64")
      case _ => None
    }
  }
}
