# Generates an XISO for xemu
# Copyright 2025 Randomcode Developers

import gatherlicenses
import os
import platform
import shutil
import subprocess
import subsystool
import sys

from argparse import ArgumentParser
from os import path


def main(argc, argv):
    repo_dir = path.abspath(path.join(path.dirname(argv[0]), ".."))

    parser = ArgumentParser(
        description="copy engine binaries to a proper release structure"
    )
    parser.add_argument(
        "--output-dir",
        "--output",
        "-o",
        dest="output",
        help="location to put the binaries",
    )
    parser.add_argument(
        "--build-dir",
        "-i",
        help="build directory to check",
        default=path.join(repo_dir, "build"),
    )
    parser.add_argument(
        "--game", "-g", help="the name of the game executable", default="FalseKing.exe"
    )
    parser.add_argument(
        "--sdk-dir", "-s", help="the path to the SDK", default=os.getenv("XDK")
    )
    parser.add_argument(
        "--xdvdfs-dir",
        "-x",
        help="the directory where xdvdfs is (default: PATH)",
        default="",
    )
    parser.add_argument("--mode", "-m", help="mode of the build", default="debug")

    args = parser.parse_args(argv[1:])

    game_name = path.splitext(args.game)[0]

    build_dir = path.join(
        os.getcwd(),
        args.build_dir,
        "xbox",
        "x86",
        args.mode,
    )
    output = args.output if args.output != None else path.join(build_dir, "xiso")

    print(f"using binaries in {build_dir}, outputting to {output}")

    game_exe = path.join(build_dir, args.game)

    subsystool_args = [
        subsystool.__file__,
        "--file",
        f"{game_exe}",
        "--subsystem",
        "XBOX",
        "--version",
        "1.00",
    ]
    subsystool.main(len(subsystool_args), subsystool_args)

    imagebld = path.join(args.sdk_dir, "xbox", "bin", "imagebld.exe")
    imagebld_args = [
        imagebld,
        f"/IN:{game_exe}",
        f"/OUT:{path.join(build_dir, f'default.xbe')}",
        f"/MAP:{path.join(build_dir, f'{game_name}.map')}",
        # f"/TITLEINFO:{path.join(repo_dir), 'public', 'xbox', 'titleinfo.xbx'}"
    ]

    if args.mode in ["debug", "release"]:
        imagebld_args += ["/DEBUG", f"/TESTNAME:{game_name}"]

    print(f"running {imagebld_args}")
    subprocess.run(imagebld_args)

    shutil.rmtree(output)
    os.mkdir(output)

    for f in os.listdir(build_dir):
        file = path.join(build_dir, f)
        if path.isfile(file):
            (name, ext) = path.splitext(f)

            out_name = f
            if ext in [".xbe"]:
                dest = path.join(output, out_name)
                if dest != None:
                    print(f"{file} -> {dest}")
                    shutil.copy(file, dest)

    xdvdfs = path.join(args.xdvdfs_dir, "xdvdfs")
    xdvdfs_args = [xdvdfs, "pack", output, path.join(build_dir, f"{game_name}.iso")]
    print(f"running {xdvdfs_args}")
    subprocess.run(xdvdfs_args)


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
