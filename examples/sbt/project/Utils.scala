object Utils {

  lazy val system = {
    System.getProperty("os.name").toLowerCase match {
      case win if win.contains("win")       => OS.Windows
      case linux if linux.contains("linux") => OS.Linux
      case mac if mac.contains("mac")       => OS.Mac
    }
  }
}

sealed trait OS {
  def name: String
}

object OS {
  object Mac extends OS {
    val name = "macos"
  }
  object Linux extends OS {
    val name = "linux"
  }
  object Windows extends OS {
    val name = "windows"
  }
}
