# Collects licenses described in licenses.toml into a folder/zip
# Copyright 2025 Randomcode Developers

import argparse
import os
import sys
import tomllib

from zipfile import ZipFile

def main(argc, argv):
    parser = argparse.ArgumentParser(description="gather third party code licenses")
    parser.add_argument("--output-dir", "--output", "-o", dest="output", help="location to put all licenses", default="licenses")
    parser.add_argument("--compress", "-c", help="compress to a zip file", default=True, action="store_true")
    parser.add_argument("--licenses", "-l", help="path to licenses.toml", default=os.path.join(os.path.dirname(argv[0]), "licenses.toml"))

    args = parser.parse_args(argv[1:])

    output = None
    if args.compress:
        output = ZipFile(f"{os.path.abspath(args.output)}.zip", "w")
    else:
        output = os.path.abspath(args.output)

    print(f"Gathering licenses in {args.licenses} to {args.output}{'.zip' if args.compress else ''}")
    licenses = tomllib.load(open(args.licenses, "rb"))
    for dep, info in licenses.items():
        print(f"{dep}: {info}")
        with open(info["license"]) as f:
            license = f.read()
        license_ext = os.path.splitext(info["license"])
        path = f"{dep}{license_ext[1] if len(license_ext[1]) else '.txt'}"
        data = f"Dependency: {info['name']}\nAuthor: {info['author']}\nUsed for: {info['use']}\nPath: {info['path']}\nURL: {info['url']}\n\nLicense:\n{license}"
        if args.compress:
            output.writestr(path, data)
        else:
            with open(os.path.join(output, path), "wb") as f:
                f.write(bytes.encode(data))


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
