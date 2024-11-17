import argparse
import os
import sys
import tomllib

from zipfile import ZipFile

def main(argc, argv):
    parser = argparse.ArgumentParser(description="gather third party code licenses")
    parser.add_argument("--output-dir", "-o", dest="output", help="location to put all licenses", default="licenses")
    parser.add_argument("--compress", "-c", help="compress to a zip file", default=True, action="store_true")
    parser.add_argument("--licenses", "-l", help="path to licenses.toml", default=os.path.join(os.path.dirname(argv[0]), "licenses.toml"))

    args = parser.parse_args()

    output = None
    if args.compress:
        output = ZipFile(f"{os.path.abspath(args.output)}.zip", "w")
    else:
        output = os.path.abspath(args.output)

    print(f"Gathering licenses in {args.licenses} to {args.output}{".zip" if args.compress else ""}")
    licenses = tomllib.load(open(args.licenses, "rb"))
    for dep, info in licenses.items():
        print(f"{dep}: {info}")
        with open(info["license"]) as f:
            license = f.read()
        path = f"{dep}.txt"
        data = f"Dependency: {info["name"]}\nAuthor: {info["author"]}\nPath: {info["path"]}\n\nLicense:\n{license}"
        if args.compress:
            output.writestr(path, data)
        else:
            with open(os.path.join(output, path), "wb") as f:
                f.write(bytes.encode(data))


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
