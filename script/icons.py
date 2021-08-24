#! /usr/bin/env python3
import common, glob, os, platform, subprocess, sys

def main():
  os.chdir(os.path.dirname(__file__) + '/../examples/resources')
  # macos only
  subprocess.check_call(["iconutil", "-c", "icns", "macos.iconset"])
  # windows, requires imagemagick
  subprocess.check_call(["convert",
                         "windows/icon_16x16.png",
                         "windows/icon_24x24.png",
                         "windows/icon_32x32.png",
                         "windows/icon_48x48.png",
                         "windows/icon_256x256.png",
                         "windows.ico"])
  return 0

if __name__ == '__main__':
  sys.exit(main())