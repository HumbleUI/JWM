#! /usr/bin/env python3
import build_utils, functools, os

basedir = os.path.abspath(os.path.dirname(__file__) + '/..')

@functools.lru_cache(maxsize=1)
def deps_compile():
  deps = [
    build_utils.fetch_maven('org.projectlombok', 'lombok', '1.18.22'),
    build_utils.fetch_maven('org.jetbrains', 'annotations', '20.1.0'),
    build_utils.fetch_maven('io.github.humbleui', 'skija-shared', '0.106.0'),
  ]

  types_dir = build_utils.get_arg("types-dir")
  if types_dir:
    deps.append(os.path.normpath(build_utils.execdir + "/" + types_dir + "/target/classes"))
  else:
    deps.append(build_utils.fetch_maven('io.github.humbleui', 'types', '0.1.1'))

  return deps

version = build_utils.get_arg("version") or build_utils.parse_ref() or build_utils.parse_sha() or "0.0.0-SNAPSHOT"