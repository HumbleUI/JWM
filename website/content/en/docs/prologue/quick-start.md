---
title: "Quick Start"
description: "One page summary of how to start a JWM project."
lead: "One page summary of how to start a JWM project."
date: 2020-11-16T13:59:39+01:00
lastmod: 2020-11-16T13:59:39+01:00
draft: false
images: []
menu:
  docs:
    parent: "prologue"
weight: 110
toc: true
---

## Install Dependencies

Download jwm jar using build tool or manually from [maven repository](https://mvnrepository.com/artifact/io.github.humbleui.jwm):

||groupId|artifactId|version|
|---|---|---|---|
|jwm|io.github.humbleUI|jwm|{{<figure src="https://img.shields.io/maven-central/v/io.github.humbleui/jwm" >}} |
|types|io.github.humbleUI|types|{{<figure src="https://img.shields.io/maven-central/v/io.github.humbleui/types" >}}|

E.g. for Maven it’ll look like this:

```xml
<dependency>
    <groupId>io.github.humbleui</groupId>
    <artifactId>jwm</artifactId>
    <version>0.3.1</version>
</dependency>
```

mvn command:

```sh
mvn dependency:get -Dartifact=io.github.humbleui:jwm:0.3.1 -Ddest=./
```

```sh
mvn dependency:get -Dartifact=io.github.humbleui:types:0.1.2 -Ddest=./
```

[coursier](https://get-coursier.io/docs/cli-fetch) command:

```sh
cs fetch io.github.humbleui:jwm:0.3.1
```

```sh
cs fetch io.github.humbleui:types:0.1.2
```

## Download example script

```sh
wget https://raw.githubusercontent.com/HumbleUI/JWM/main/docs/GettingStarted.java
```

## Run Example

On Windows and Linux:

```sh
java -cp jwm-0.3.1.jar:types-0.1.1.jar GettingStarted.java
```

On macOS, add `-XstartOnFirstThread` flag to `java`:

```sh
java -XstartOnFirstThread -cp jwm-0.3.1.jar:types-0.1.1.jar GettingStarted.java
```

### note

`types-0.1.1.jar` here are from [https://github.com/HumbleUI/Types](https://github.com/HumbleUI/Types). They will be included as a transitive dependency if you are using Maven or Gradle.

## Step by Step

### Initialize

Init JWM library:

```java
App.init();
```

### Creating a window

Create a window:

```java
Window window = App.makeWindow();
window.setTitle("Hello, world!");
```

Set up a listener:

```java
class EventHandler implements Consumer<Event> {
    public final Window window;

    public EventHandler(Window window) {
        this.window = window;
    }
}
```

Write the `accept` function:

```java
@Override
public void accept(Event e) {
    System.out.println(e);

    if (e instanceof EventWindowCloseRequest) {
        window.close();
        App.terminate();
    }
}
```

Assign handler to the window:

```java
window.setEventListener(new EventHandler(window));
```

Display the window:

```java
window.setVisible(true);
```

### start an event loop

Start the event loop (will block the thread):

```java
App.start();
```

See it all together in [GettingStarted.java](GettingStarted.java).
