"""Keep Bluepad32 compiler arguments below Windows' process command limit."""

import os

Import("env")


def short_include_paths(build_env, node):
    # Keep CPPPATH intact for SCons' header dependency scanner. Only shorten
    # the flags passed to GCC, using its common include-directory prefix.
    framework = build_env.PioPlatform().get_package_dir("framework-arduinoespressif32")
    prefix = framework.replace("\\", "/").rstrip("/") + "/"
    flags = ["-iprefix", prefix]
    for directory in build_env.get("CPPPATH", []):
        path = build_env.Dir(directory).get_abspath().replace("\\", "/")
        if path.lower().startswith(prefix.lower()):
            flags.extend(["-iwithprefixbefore", path[len(prefix):]])
        else:
            flags.extend(["-I", path])
    return build_env.Object(node, _CPPINCFLAGS=flags)


if os.name == "nt":
    env.AddBuildMiddleware(short_include_paths)
