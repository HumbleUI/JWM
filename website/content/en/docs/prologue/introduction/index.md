---
title: "Introduction"
description: "JWM is a cross-platform Java window management and OS integration library."
lead: "JWM is a cross-platform Java window management and OS integration library."
date: 2020-10-06T08:48:57+00:00
lastmod: 2020-10-06T08:48:57+00:00
draft: false
images: []
menu:
  docs:
    parent: "prologue"
weight: 100
toc: true
---

{{< alert icon="👉" text="Status: Alpha. Expect API breakages." />}}

{{<figure src="./jwm.gif">}}

## Getting Started

||groupId|artifactId|version|
|---|---|---|---|
|jwm|io.github.humbleUI|jwm|{{<figure src="https://img.shields.io/maven-central/v/io.github.humbleui/jwm" >}} |

For maven

```xml
<dependency>
    <groupId>io.github.humbleui</groupId>
    <artifactId>jwm</artifactId>
    <version>0.3.1</version>
</dependency>
```

For gradle

```gradle
dependencies {
    compile 'io.github.humbleui:jwm:0.3.1'
}
```

For leiningen

```lein
dependencies [[org.clojure/clojure "x.y.z"]
              [io.github.humbleui/jwm "0.3.1"]]
```

For sbt

```scala
libraryDependencies += "io.github.humbleui" % "jwm" % "0.3.1"
```

### Tutorial

Check [Quick Start →]({{< relref "quick-start" >}})

### Recipes

Check [Recipes →]({{< relref "recipes" >}})

### Examples

Visit [examples direcory](https://github.com/HumbleUI/JWM/tree/main/examples) in JWM repository. As JWM intends to play well with skija, [examples in Skija repository](https://github.com/HumbleUI/Skija/tree/master/examples) will also help.

## Contributing & Development

### Convensions

Learn convensions and API design of JWM. [Convension Guide →]({{< relref "convensions" >}})

### contribution guide

PRs & issue reports are welcome!

Please read [Convensions]({{< relref "convensions" >}}) first.

If you are looking where to start, there’s a label: [Good first issue](https://github.com/HumbleUI/JWM/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22).

Issues labeled “[Design](https://github.com/HumbleUI/JWM/issues?q=is%3Aissue+is%3Aopen+label%3ADesign)” and “[Not sure](https://github.com/HumbleUI/JWM/issues?q=is%3Aissue+is%3Aopen+label%3A%22Not+sure%22)” require prior discussion—leave a comment with your ideas!

## Help

Get help on JWM. [Help →]({{< relref "troubleshooting" >}})

## Resources

### frameworks

- Clojure Desktop UI framework: [https://github.com/HumbleUI/HumbleUI](https://github.com/HumbleUI/HumbleUI)
