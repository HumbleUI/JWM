---
title: "Troubleshooting"
description: "Solutions to common problems."
lead: "Solutions to common problems."
date: 2020-11-12T15:22:20+01:00
lastmod: 2020-11-12T15:22:20+01:00
draft: false
images: []
menu: 
  docs:
    parent: "help"
weight: 620
toc: true
---

## General

- run `python ./script/crean.py`
- try removing `<platform>`/build directory and re-run command
- check environment variables and paths(`which <cmd>` in unix-like shell or (`echo`, `where <cmd>` in Windows power shell)

### Could not run example application

#### check prerequisites

- Example application needs [JDK 16](https://jdk.java.net/16/)

## Problems on Windows

### build fails

- check [development/setup-win](({{< relref "setup-win" >}}))
- If you use Windows, make sure you disable other c++ compilers like minGW

## Problems on Mac OS

tbd

## Problems on Linux

tbd
