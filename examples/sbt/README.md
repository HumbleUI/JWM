# example

|OS and arch \\ packaging |graalvm|jpackage|
|---|---|---|
|Windows 10 amd64|✅|✅|
|linux(ubuntu 20.04) amd64|✅|✕|
|linux(ubuntu 18.04) amd64|???|✅|
|mac amd64|???|???|
|mac arm64|???|???|
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
