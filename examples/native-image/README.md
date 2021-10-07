# example

|OS and arch \\ packaging |graalvm|jpackage|
|---|---|---|
|Windows 10 amd64|✅|✅|
|linux(ubuntu 20.04) amd64|✅|✕|
|linux(ubuntu 18.04) amd64|???|✅|
|mac amd64|???|???|
|mac arm64|???|???|

## Install sbt

|system,os|tool|command|note|
|---|---|---|---|
|Linux |coursier|1. `curl -fLo cs https://git.io/coursier-cli-"$(uname \| tr LD ld)"` <br/> 2. `chmod +x cs` <br/>3. `cs setup` or `cs install sbt`|https://get-coursier.io/docs/cli-installation#linux-macos|
|Ubuntu|apt|1. `echo "deb https://repo.scala-sbt.org/scalasbt/debian all main" \| sudo tee /etc/apt/sources.list.d/sbt.list`<br>2. `echo "deb https://repo.scala-sbt.org/scalasbt/debian /" \| sudo tee /etc/apt/sources.list.d/sbt_old.list` <br>3. `curl -sL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2EE0EA64E40A89B84B2DF73499E82A75642AC823" \| sudo apt-key add`<br> 4. `sudo apt update`<br/>5.`sudo apt install sbt` |https://www.scala-sbt.org/1.x/docs/Installing-sbt-on-Linux.html|
|Windows| choco | `choco install sbt`|https://www.scala-sbt.org/1.x/docs/Installing-sbt-on-Windows.html|
|Windows| scoop | `scoop install sbt`|https://www.scala-sbt.org/1.x/docs/Installing-sbt-on-Windows.html|
|Windows | coursier|1. `bitsadmin /transfer cs-cli https://git.io/coursier-cli-windows-exe "%cd%\cs.exe"` <br/>2. `.\cs setup`|https://get-coursier.io/docs/cli-installation#windows|
|Mac amd64|brew|`brew install sbt`| https://www.scala-sbt.org/1.x/docs/Installing-sbt-on-Mac.html|
|Mac arm64| - |1. download universal package from https://github.com/sbt/sbt/releases/download/v1.5.5/sbt-1.5.5.zip<br/>2. `unzip sbt-1.5.2.zip`<br/>3. `sudo mv your/path/to/sbt/bin/sbt /usr/local/bin` |
|Mac arm64|sdkman|1. `curl -s "https://get.sdkman.io" \| bash`<br>2. `sdk install sbt` |https://stackoverflow.com/questions/66464412/install-scala-to-apple-silicon|
## run example application

```sh
// at jwm dir
./script/build.py
./script/package.py --ref refs/tag/x.y.z-SNAPSHOT --publish-local
cd examples/native-image
vim project/Dependencies.scala
```

```diff
object Dependencies {
  val skijaVersion = "0.93.1"
  // ...
+   val jwmVersion = "x.y.z-SNAPSHOT"
```

```sh
sbt run
```
## build graal native image

```sh
sbt nativeImageCommand nativeImageRunAgent
sbt nativeImage
// on linux
./target/native-image/example
// on Windows
./target/native-image/example.exe
```

## build jpackage

```sh
sbt packageApp
// package will be saved at target/windows
```
