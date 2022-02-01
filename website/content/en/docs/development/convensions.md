---
title: "Convensions"
description: "Convensions and API design of JWM."
lead: "Convensions and API design of JWM."
date: 2020-10-13T15:21:01+02:00
lastmod: 2020-10-13T15:21:01+02:00
draft: false
images: []
menu:
  docs:
    parent: "development"
weight: 130
toc: true
---

## General naming conventions

- camelCase in Java & C++.
- UPPER_CASE for enum values.
- Field/method prefixes:
  - `_n...` for native methods.
  - `_...`  for “private” fields.
  - `get...`/`is...` for getters.
  - `set...`/`with...` for setters.
  - `make...` for static named constructors.
  - `_FLAG_<TYPE>_<NAME>` for bit flags.
  - `...Mask` for bit masks.
- Getter methods ALWAYS start with `get...` / `is...` (`lineNumber` -> `getLineNumber()`, `accessible` -> `isAccessible()`).
- Constructors/static builders ALWAYS start with `make...`.
- Setters/updaters must return `this`.
- Class names use `<Generic>-<Specific>` pattern (`EventKey`, not `KeyEvent`, `WindowMac`, not `MacWindow`). That way they group nicely in file lists.

Some common dictionary:

- ptr (paintPtr, canvasPtr, ...)
- count (instead of size/length/...)

## Code organization

- Use `*.cc` and `*.hh` for C++ files
- All C++ code in `jwm` namespace
- All public Java code in `io.github.humbleui.jwm` namespace, implementation in `io.github.humbleui.jwm.impl`
- No inner classes

## Visibility

- All fields/methods `public`.
- Fields/methods/inner classes not for public consumption:
  - also `public`, but prefixed with `_` (`startIndex` -> `_startIndex`).
  - annotated with @ApiStatus.Internal

Why public?

- We cannot anticipate what specific needs our clients might have. Sometimes the difference between totally possible and completely impossible might be as small as a single field made public.
- It’s strictly better to have implementation details accessible and not use them than just have them inaccessible.
- Multiple packages are impossible even with protected visibility.
- We are all grown ups. If clients use fields starting with `_`, we assume they know what they are doing, and that the alternatives were worse.
- As with private APIs, we give no guarantees with regards to `_` fields/methods. They are considered effectively private, but if you really, really need them, they are here for you.

## Data classes

- public final fields prefixed with `_`.
- public getters/setters following javaBeans convention (get/is/set/with).
- setters return this.
- flags/bit masks are not exposed as getters, instead, individual check for each flag value is done.

Why getters/setters?

- If fields goes away if future version, still can be emulated with getter (or getter might throw).
- Computable fields need getters (e.g. height = bottom - top), would be strange to have them as methods but others as fields.
- Some fields need to call native instance, again, getter can hide that.

Example:

```java
@lombok.Data
public class LineMetrics {
    @ApiStatus.Internal public final long    _startIndex;
    @ApiStatus.Internal public final long    _endIndex;
    @ApiStatus.Internal public final long    _endExcludingWhitespaces;
    @ApiStatus.Internal public final long    _endIncludingNewline;
    @ApiStatus.Internal public final boolean _hardBreak;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _flags;

    @ApiStatus.Internal public static final int _FLAG_IS_BOLD   = 0b0001;
    @ApiStatus.Internal public static final int _FLAG_IS_ITALIC = 0b0010;
    public boolean isBold() { return (_flags | _FLAG_IS_BOLD) != 0; }
    public boolean isItalic() { return (_flags | _FLAG_IS_ITALIC) != 0; }
}
```

## API conventions

- Data types:
  - Fractional: float (Java) / float (C++)
  - String indices: int (Java) / int32_t/jint (C++)
  - Array indices: int (Java) / int32_t/jint (C++)
  - Pointers: long (Java) / int64_t/jlong (C++)
- (0, 0) in top left corner
- Use unscaled (physical) pixels for everything (mouse position, windows size, etc). DPI scaling is a problem for API clients
- DPI is fractional (1.5, 1.75)

## Interop

- Native methods are `public static`, name starts with `_n...` + java method name.
  - Exceptions are overloaded Skia methods. Can’t overload in JNI.
- Enum values are passed as ints (for this to work, enums must have same elements in the same order).
- Unroll objects and small fixed-size arrays when possible (Rect -> jfloat, jfloat, jfloat, jfloat, int[2] -> jint, jint).
- Pointers are passed as longs.
- Two-integers are returned as single long.
- For anything bigger, return data objects, e.g. IRect instead of int[].
- Strings are passed as `jstring` (converted to UTF-8/UTF-16 on C++ side).
- Prefer `Native.ptr(obj)` to `obj._ptr` (required for nullable pointers, advised for the rest for consistency).
- Assert known constraint on Java side (e.g. `assert matrix.length == 9`).
