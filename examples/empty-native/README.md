# Gradle build scripts  for `empty` example application

This directory contains Gradle scripts to build the `empty` example application,
especially to produce a native image of the application using the
[GraalVM](https://www.graalvm.org/latest/reference-manual/native-image/) tool.

The scripts assume you already have the [Gradle](https://gradle.org/releases/) build tool
installed and is available in the path. Standard Gradle commands can then be used to build the
`empty` application, such as:

* `gradle run`: compile and run the application
* `gradle jar`: generate the application's `jar` file in `build/libs/` directory
* etc.

By default, the locally-compiled JWM classes are used as dependencies to compile the `empty`
application. To use a published JWM version in Maven repository, specify the `-PjwmVersion=...`
option in the Gradle commands, e.g.

* `gradle run -PjwmVersion=0.4.25`: compile and run the application with JWM `0.4.25` version
  published in Maven repository
* etc.

All of the above build commands would work on all platforms supported by JWM.

Especially the scripts make use of the latest
[Gradle native image](https://graalvm.github.io/native-build-tools/latest/gradle-plugin.html)
plugin to generate the `empty-native` executable for current platform, as detailed below.

## Building `empty` native image

### Prerequisites

A GraalVM JDK 25+ distribution is required to be installed and is available in the system, 
e.g. from [Oracle](https://www.graalvm.org/downloads/) release, or from a
[GraalVM Community](https://github.com/graalvm/graalvm-ce-builds/releases) distribution
or from a [Liberica Native Image Kit](https://bell-sw.com/liberica-native-image-kit/) release,
with the JAVA_HOME environment variable pointing to the installation and its `bin` directory be
added to the path. More detailed installing info for various OSes is
[here](https://www.graalvm.org/latest/getting-started/#installing).

A C compiler for the current platform is also required to produce the native image executable.
Detailed setup info is available [here](https://www.graalvm.org/latest/reference-manual/native-image/#prerequisites).

### Building native image

First, the
[native image metadata](https://www.graalvm.org/latest/reference-manual/native-image/metadata/AutomaticMetadataCollection/)
for the `empty` application need be generated, by running the application with the `-Pagent` option:
```
gradle run -Pagent
```
Use and interact with the running `empty` application as per normal, then close it.

Then generate its native image with the command:
```
gradle nativeCompile
```
The resulting native image will be produced in the `build/native/nativeCompile/` directory,
and can be executed directly:
```
./build/native/nativeCompile/empty-native
```
or in Windows:
```
build\native\nativeCompile\empty-native.exe
```

### Supported platforms

The `empty` native images can be produced on platforms supported by GraalVM, namely:

- Linux `x64/arm64`,
- MacOS `x64/arm64` (but the `x64` support is removed in GraalVM 25.0.2+),
- Windows `x64`.
