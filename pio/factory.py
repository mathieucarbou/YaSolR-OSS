# Part of ESPEasy build toolchain.
#
# Combines separate bin files with their respective offsets into a single file
# This single file must then be flashed to an ESP32 node with 0 offset.
#
# Original implementation: Bartłomiej Zimoń (@uzi18)
# Maintainer: Gijs Noorlander (@TD-er)
#
# Special thanks to @Jason2866 (Tasmota) for helping debug flashing to >4MB flash
# Thanks @jesserockz (esphome) for adapting to use esptool.py with merge_bin
#
# Typical layout of the generated file:
#    Offset | File
#    0x1000 | bootloader.bin
#    0x8000 | partitions.bin
#    0xe000 | boot_app0.bin
#   0x10000 | firmware.bin
#  0x3F0000 | littlefs.bin

Import("env", "projenv")

import sys
from os.path import join, getsize

sys.path.append(join(env.PioPlatform().get_package_dir("tool-esptoolpy")))
import esptool

# print(env.Dump())
# print(projenv.Dump())

def esp32_create_combined_bin(source, target, env):
    print("Generating combined binary for serial flashing")

    # env.Execute('pio run -t buildfs -e %s' % env['PIOENV'])

    # The offset from begin of the file where the app0 partition starts
    # This is defined in the partition .csv file
    app_offset = 0x10000
    fs_offset = 0x0

    if env.get("PARTITIONS_TABLE_CSV").endswith("partitions-4MB.csv"):
        fs_offset = 0x3F0000
    if env.get("PARTITIONS_TABLE_CSV").endswith("partitions-8MB.csv"):
        fs_offset = 0x7D0000

    if fs_offset == 0:
        print(env.get("PARTITIONS_TABLE_CSV"))
        raise Exception("Unknown partition file, cannot determine FS offset")

    new_file_name = env.subst("$BUILD_DIR/${PROGNAME}.factory.bin")
    sections = env.subst(env.get("FLASH_EXTRA_IMAGES"))
    firmware_name = env.subst("$BUILD_DIR/${PROGNAME}.bin")
    fs_name = env.subst("$BUILD_DIR/littlefs.bin")
    chip = env.get("BOARD_MCU")
    flash_size = env.BoardConfig().get("upload.flash_size")
    flash_freq = env.BoardConfig().get("build.f_flash", "40m")
    flash_freq = flash_freq.replace("000000L", "m")
    flash_mode = env.BoardConfig().get("build.flash_mode", "dio")
    memory_type = env.BoardConfig().get("build.arduino.memory_type", "qio_qspi")
    if flash_mode == "qio" or flash_mode == "qout":
        flash_mode = "dio"
    if memory_type == "opi_opi" or memory_type == "opi_qspi":
        flash_mode = "dout"
    cmd = [
        "--chip",
        chip,
        "merge_bin",
        "-o",
        new_file_name,
        "--flash_mode",
        flash_mode,
        "--flash_freq",
        flash_freq,
        "--flash_size",
        flash_size,
    ]

    # platformio estimates the amount of flash used to store the firmware. this
    # estimate is not accurate. we perform a final check on the firmware bin
    # size by comparing it against the respective partition size.
    max_size = env.BoardConfig().get("upload.maximum_size", 1)
    fw_size = getsize(firmware_name)
    if fw_size > max_size:
        raise Exception("firmware binary too large: %d > %d" % (fw_size, max_size))

    print("    Offset | File")
    for section in sections:
        sect_adr, sect_file = section.split(" ", 1)
        print(f" -  {sect_adr} | {sect_file}")
        cmd += [sect_adr, sect_file]

    print(f" - {hex(app_offset)} | {firmware_name}")
    cmd += [hex(app_offset), firmware_name]

    # print(f" - {hex(fs_offset)} | {fs_name}")
    # cmd += [hex(fs_offset), fs_name]

    print("Using esptool.py arguments: %s" % " ".join(cmd))

    esptool.main(cmd)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", esp32_create_combined_bin)
