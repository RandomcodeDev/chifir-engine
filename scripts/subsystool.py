import pefile
import shutil
import sys

from argparse import ArgumentParser
from os import path


def main(argc, argv):
    parser = ArgumentParser(description="Change the subsystem of a Windows executable")
    parser.add_argument("--file", "-f", required=True, help="The PE file to modify")
    parser.add_argument("--subsystem", "-s", help="The new subsystem", default="XBOX")
    parser.add_argument(
        "--version", "-v", help="The new subsystem version", default="1.00"
    )

    args = parser.parse_args(argv[1:])

    exe = args.file
    if not path.isfile(exe):
        print(f"{exe} isn't a file or wasn't found!")
    subsys = args.subsystem.upper().strip()
    version = args.version.strip().split(".")
    major = int(version[0])
    minor = int(version[1]) if len(version) > 1 else 0

    pe = pefile.PE(exe)

    if f"IMAGE_SUBSYSTEM_{subsys}" not in pefile.SUBSYSTEM_TYPE:
        print(f"Unknown subsystem {subsys}!")
        exit()

    print(
        f"changing subsystem of {exe} from {pefile.SUBSYSTEM_TYPE[pe.OPTIONAL_HEADER.Subsystem]} "
        + f"{pe.OPTIONAL_HEADER.MajorSubsystemVersion}.{pe.OPTIONAL_HEADER.MinorSubsystemVersion:02}"
        + f" to IMAGE_SUBSYSTEM_{subsys} {major}.{minor:02}"
    )

    pe.OPTIONAL_HEADER.Subsystem = pefile.SUBSYSTEM_TYPE[f"IMAGE_SUBSYSTEM_{subsys}"]
    pe.OPTIONAL_HEADER.MajorSubsystemVersion = major
    pe.OPTIONAL_HEADER.MinorSubsystemVersion = minor

    data = pe.write()
    pe.close()
    with open(exe, "wb+") as f:
        f.write(data)


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
