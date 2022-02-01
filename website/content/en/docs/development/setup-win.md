---
title: "Setup Guide on Windows"
description: "How to Setup Dev Environment for JWM on Windows"
lead: "We explain how to setup dev environment for JWM on Windows in detail here."
date: 2020-10-06T08:49:31+00:00
lastmod: 2020-10-06T08:49:31+00:00
draft: false
images: []
menu:
  docs:
    parent: "development"
weight: 150
toc: true
---
## Install Python 3

from windows power shell

```sh
winget install Python.Python.3
```

or from chocolatey

```sh
choco install -y python
python --version
```

## Install Ninja

Download executable from [https://github.com/ninja-build/ninja/releases](https://github.com/ninja-build/ninja/releases) and export path.

## Install visual studio build tools

Run this command in windows power shell.

```sh
winget install Microsoft.VisualStudio.2019.BuildTools
```

Then, run `C:\Program Files (x86)\Microsoft Visual Studio\Installer\setup.exe` and install c++ development tools.

After reboot, open one of the command prompts for VS 2019 matching your arch.

Usually, you can open command prompt for VS from `C:\path\to\Start Menu\Programs\Visual Studio 2019\Visual Studio Tools\VC`.

### Configure path and env (optional)

If you want to use visual studio build tools not from
command prompt for VS but from another shell like [git bash](https://gitforwindows.org/), export these environment variables.

NOTE: Change paths, architecture(arm,x64,x86) and visual studio version according to your environment.

INCLUDE

- C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30133\include
- C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt
- C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared
- C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\cppwinrt
- C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um
- C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\winrt

LIB

- C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\ucrt\x64
- C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\ucrt_enclave\x64
- C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64
- C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30133\lib\x64

PATH

- C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\x64
- C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin
- C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64
- C:\path\to\ninja
