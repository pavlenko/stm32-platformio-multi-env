# TODO update src_filter of stm32 script like below in stm8
# https://github.com/platformio/platform-ststm8/blob/712f391f99a6b767fe7f1183fdb2b7d4d239b549/builder/frameworks/spl.py#L92-L96
# https://github.com/platformio/platform-ststm32/issues/568
# https://docs.platformio.org/en/latest/scripting/middlewares.html
import os
import sys

from pprint import pprint
from platformio.proc import exec_command

Import("env")

platform = env.PioPlatform()
board = env.BoardConfig()
config = env.GetProjectConfig()

MCU = board.get("build.mcu", "")
MCU_FAMILY = MCU[0:7]
FRAMEWORK_DIR = platform.get_package_dir("framework-stm32cube%s" % MCU[5:7])
TOOLCHAIN_DIR = platform.get_package_dir("toolchain-gccarmnoneeabi")

hal_driver_dir = os.path.join(FRAMEWORK_DIR, "Drivers", MCU_FAMILY.upper() + "xx_HAL_Driver")
hal_config_name = MCU_FAMILY + "xx_hal_conf.h"

print("HAL compilation speedup")
print("HAL DRIVER DIR: " + hal_driver_dir)
print("HAL CONFIG NAME: " + hal_config_name)

if board.get("build.stm32cube.custom_config_header", "no") == "yes":
    hal_config_path = os.path.join(env["PROJECT_DIR"], "env", "bootloader", "src", hal_config_name)
else:
    hal_config_path = os.path.join(hal_driver_dir, "Inc", hal_config_name)

print("HAL CONFIG PATH: " + hal_config_path)

src_include_only = None


def resolve_src_include_only():
    global src_include_only

    if not os.path.isfile(hal_config_path):
        sys.stderr.write("Error: Couldn't find " + hal_config_path + " file!\n")
        src_include_only = []

    if src_include_only is None:
        command = [TOOLCHAIN_DIR + "/bin/" + env.subst("$CC")]
        command += env.subst("${TEMPFILE('$CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES','$CCCOMSTR')}").split(' ')
        command += ["-W", "-MM", "-E", hal_config_path]

        result = exec_command(command, cwd=TOOLCHAIN_DIR + "/bin", env=env['ENV'])
        if result['returncode'] != 0:
            sys.stderr.write("Error: Could not parse library files for the target.\n")
            sys.stderr.write(result['err'])
            env.Exit(1)

        src_include_only = []
        includes = result['out']
        for inc in includes.split(" "):
            if "_" not in inc or ".h" not in inc or "conf" in inc:
                continue
            inc = inc.strip()
            inc = inc.replace(".h", ".c")
            inc = inc.replace(
                os.path.join(hal_driver_dir, "Inc"),
                os.path.join(hal_driver_dir, "Src"),
            )
            src_include_only.append(inc)


def middleware(env, node):
    resolve_src_include_only()
    if not src_include_only:
        return node

    if not str(node).startswith(hal_driver_dir):
        return node

    if not str(node) in src_include_only:
        return None

    return node


env.AddBuildMiddleware(middleware)
