import argparse
import os
import re
import shlex
import subprocess
import sys

from subprocess import Popen


def main(argc, argv):
    root = os.path.join(
        os.path.abspath(os.path.join(os.path.dirname(argv[0]), "..")), ""
    )

    parser = argparse.ArgumentParser(
        description="execute a command on all files matching certain parameters in the repo"
    )
    parser.add_argument(
        "--exclude",
        "-e",
        help="folders to exclude (separate with semicolons)",
        default=".cache;.git;external;projects;obj;out",
    )
    parser.add_argument(
        "--extensions",
        "-t",
        help="file extensions to include (separate with semicolons)",
        default=".cpp;.h",
    )
    parser.add_argument(
        "--regex", "-r", help="regex to additionally filter files", default=""
    )
    parser.add_argument(
        "--confirm",
        "-y",
        help="execute the commands instead of just printing them",
        default=False,
        action="store_true",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        help="show output and return codes of commands",
        default=False,
        action="store_true",
    )
    parser.add_argument(
        "--command",
        "-c",
        default=f"clang-format -i --style=file:{os.path.join(root, ".clang-format")}",
    )

    args = parser.parse_args(argv[1:])

    excludes = args.exclude.split(";")
    extensions = args.extensions.split(";")
    regex_str = args.regex
    confirm = args.confirm
    verbose = args.verbose
    command = shlex.split(args.command, posix=(sys.platform != "win32"))

    regex = re.compile(regex_str)

    print(
        f'Running {command} on all {extensions} files matching regex "{regex_str}" in "{root}" not in {excludes}'
    )

    for dir, _, files in os.walk(root):
        dir = dir.removeprefix(root)
        excluded = False
        for exclude in excludes:
            if dir.startswith(exclude):
                excluded = True
        if not excluded:
            for file in files:
                excluded = False
                if len(extensions) > 0 and not (
                    os.path.splitext(file)[1] in extensions
                ):
                    excluded = True
                if len(regex_str) and not regex.match(os.path.join(dir, file)):
                    excluded = True
                if not excluded:
                    args = command + [os.path.join(root, dir, file)]
                    print(args)
                    if confirm:
                        proc = Popen(args, stdout=subprocess.PIPE, cwd=root)
                        stdout, stderr = proc.communicate()
                        if verbose:
                            print(
                                f"returned {proc.returncode}\n\nstdout:\n{stdout}\n\nstderr:\n{stderr}"
                            )


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
