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


# STM8 version
# env.BuildSources(
#     join("$BUILD_DIR", "SPL"),
#     join(FRAMEWORK_DIR, "Libraries", "STM8S_StdPeriph_Driver", "src"),
#     src_filter=["-<*>"] + [" +<%s>" % f for f in get_core_files()]
# )

def get_core_files():
    # system_conf = os.path.join(
    #     FRAMEWORK_DIR,
    #     "Drivers",
    #     MCU_FAMILY + "xx_HAL_Driver",
    #     "Inc",
    #     MCU_FAMILY + "xx_hal_conf.h"
    # )
    project_conf = os.path.join(env["PROJECT_DIR"], "env", "bootloader", "src", MCU_FAMILY + "xx_hal_conf.h")

    # TODO check which HAL config to check
    # Check if project HAL config exists
    if not os.path.isfile(project_conf):
        sys.stderr.write("Error: Couldn't find " + project_conf + " file!\n")
        return []

    # Create command for resolve required headers
    command = [TOOLCHAIN_DIR + "/bin/" + env.subst("$CC")]
    command += env.subst("${TEMPFILE('$CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES','$CCCOMSTR')}").split(' ')
    command += ["-W", "-MM", "-E", project_conf]

    result = exec_command(
        command,
        cwd=TOOLCHAIN_DIR + "/bin",
        env=env['ENV']
    )

    if result['returncode'] != 0:
        sys.stderr.write("Error: Could not parse library files for the target.\n")
        sys.stderr.write(result['err'])
        env.Exit(1)

    src_files = []
    includes = result['out']
    for inc in includes.split(" "):
        if "_" not in inc or ".h" not in inc or "conf" in inc:
            continue
        src_files.append(os.path.basename(inc).replace(".h", ".c").strip())
        # src_files.append(inc.replace(".h", ".c").strip())

    pprint(src_files)
    # env.Exit(0)
    return src_files


# TODO init all required vars
#      add src_filter array and fill it only when requested
# get_core_files()

# pprint(vars(env))
# env.Exit(0)


# STM32 version
# do not call this function, use other possibility
# env.BuildSources(
#     os.path.join("$BUILD_DIR", "FrameworkHALDriver"),
#     os.path.join(FRAMEWORK_DIR, "Drivers",  MCU_FAMILY.upper() + "xx_HAL_Driver"),
#     src_filter=["-<*> -<Src/*_template.c> -<Src/Legacy>"] + [" +<%s>" % f for f in get_core_files()]
# )


def middleware(env, node):
    # TODO check if node path like HAL system config path
    # TODO if yes - check if file in allowed list, else return null
    print(node)
    return node


env.AddBuildMiddleware(middleware)
