---
title: "Run Example Applcation"
description: "How to Run JWM Example Applcation."
lead: "How to Run JWM Example Applcation"
date: 2020-10-06T08:49:31+00:00
lastmod: 2020-10-06T08:49:31+00:00
draft: false
images: []
menu:
  docs:
    parent: "development"
weight: 135
toc: true
---


{{<figure src="./screenshot.png" >}}

## Prerequisites

First of all, download and install these dependencies below.

- Shared: Git, CMake(3.11+), Ninja, C++ compiler, JDK 11+, $JAVA_HOME, Python 3
- Windows 10: Microsoft Visual C++ (MSVC), x64 Native Tools Command Prompt for VS.
- Ubuntu 20.04: libxcomposite-dev libxrandr-dev libgl1-mesa-dev libxi-dev libxcursor-dev

### Clone repository

```sh
git clone git@github.com:humbleui/jwm.git
cd jwm
```

### Run dashboard example app without building:

```sh
./script/run.py --jwm-version <version>
```

### Or build from source and run

```sh
./script/run.py
```

### Run native-image example app:

```sh
todo
```

## other commands

### build from source

```sh
./script build
```

### clean build

```sh
./script clean
```
