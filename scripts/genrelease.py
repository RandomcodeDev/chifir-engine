# Generates a release zip for PC
# Copyright 2025 Randomcode Developers

import gatherlicenses
import os
import platform
import shutil
import sys

from argparse import ArgumentParser
from os import path
from zipfile import ZipFile


def python_platform_to_xmake(plat: str = platform.system()) -> str:
	match plat:
		case "Darwin":
			return "macosx"
		case _:
			return plat.lower()


def python_machine_to_xmake(machine: str = platform.machine()) -> str:
	match machine:
		case "AMD64" | "x86_64":
			return "x64"
		case "i386" | "i686":
			return "x86"
		case "ARM64" | "aarch64":
			return "arm64"
		case _:
			return machine.lower()


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
		"--compress",
		"-c",
		help="compress to a zip file",
		default=False,
		action="store_true",
	)
	parser.add_argument(
		"--force",
		"-f",
		help="don't prompt to delete old install dir",
		default=False,
		action="store_true",
	)
	parser.add_argument(
		"--static",
		"-s",
		help="ensure symbols from a static build are copied",
		default=False,
		action="store_true"
	)
	parser.add_argument(
		"--build-dir", "-i", help="build directory to check", default=path.join(repo_dir, "build")
	)
	parser.add_argument(
		"--plat", "-p", help="platform of the build", default=platform.system()
	)
	parser.add_argument(
		"--arch", "-a", help="architecture of the build", default=platform.machine()
	)
	parser.add_argument(
		"--game", "-g", help="the name of the game executable", default="FalseKing"
	)
	parser.add_argument("--mode", "-m", help="mode of the build", default="debug")

	args = parser.parse_args(argv[1:])

	plat = python_platform_to_xmake(args.plat)
	if plat not in ["windows", "gdk", "gdkx", "linux"]:
		print(
			f"genrelease is not designed for {plat}, there's probably another one specific to it (makensp, etc)"
		)

	build_dir = path.join(
		os.getcwd(),
		args.build_dir,
		"windows" if plat == "gdk" else plat,
		python_machine_to_xmake(args.arch),
		args.mode,
	)
	output = (
		args.output if args.output != None else path.join(args.build_dir, "install")
	)
	print(f"copying binaries in {build_dir} to {output}")

	if path.exists(output):
		try:
			resp = "y" if args.force else input(f"remove directory {output} (yes or no)? ")
			if resp.lower()[0] == "n":
				print("not removing")
				exit()
			else:
				shutil.rmtree(output)
		except KeyboardInterrupt:
			print("\nnot removing")
			exit()

	os.mkdir(output)
	os.mkdir(path.join(output, "bin"))

	# only Windows, Xbox, and Linux are non-static
	EXE_EXTS = {"windows": ".exe", "gdk": ".exe", "gdkx": ".exe", "linux": None}
	DLL_EXTS = {
		"windows": ".dll",
		"gdk": ".dll",
		"gdkx": ".dll",
		"linux": ".so",
	}
	SYM_EXTS = {"windows": ".pdb", "gdk": ".pdb", "gdkx": ".pdb", "linux": None}

	for root, _, files in os.walk(build_dir):
		for f in files:
			file = path.join(root, f)
			if path.isfile(file):
				(name, ext) = path.splitext(f)
				dest = None
				syms = None
				if name == path.splitext(args.game)[0] and ext == EXE_EXTS[plat]:
					dest = output
				elif ext == DLL_EXTS[plat]:
					dest = path.join(output, "bin")
				elif ext == SYM_EXTS[plat] and args.static:
					dest = output

				syms = path.join(build_dir, f"{name}{SYM_EXTS[plat]}")

				if dest != None:
					print(f"{file} -> {path.join(dest, f)}")
					shutil.copy(file, dest)
					if path.exists(syms) and syms != file:
						print(f"{syms} -> {path.join(dest, path.basename(syms))}")
						shutil.copy(syms, dest)

	if plat == "windows":
		d3d12_agility = path.join(repo_dir, "external", "d3d12_agility")
		for f in os.listdir(d3d12_agility):
			file = path.join(d3d12_agility, f)
			if path.isfile(file):
				(name, ext) = path.splitext(f)
				if ext == DLL_EXTS[plat]:
					dest = path.join(output, "bin")
					print(f"{file} -> {path.join(dest, f)}")
					shutil.copy(file, dest)
		gdk_meta = path.join(repo_dir, "public", "gdk")
		for root, _, files in os.walk(gdk_meta):
			for f in files:
				file = path.join(root, f)
				if path.isfile(file):
					(name, ext) = path.splitext(f)
					dest = root.replace(gdk_meta, output)
					if ext != ".mgc":
						print(f"{file} -> {path.join(dest, f)}")
						if not path.exists(dest):
							os.mkdir(dest)
						shutil.copy(file, path.join(dest, f))

	if plat in ["gdk", "gdkx"]:
		gdk_meta = path.join(repo_dir, "public", "gdk")
		for root, _, files in os.walk(gdk_meta):
			for f in files:
				file = path.join(root, f)
				if path.isfile(file):
					(name, ext) = path.splitext(f)
					dest = root.replace(gdk_meta, output)
					if f != "AppxManifest.xml":
						if ext == ".mgc":
							f = "MicrosoftGame.config"
						print(f"{file} -> {path.join(dest, f)}")
						if not path.exists(dest):
							os.mkdir(dest)
						shutil.copy(file, path.join(dest, f))

	gatherlicenses_args = [f'{gatherlicenses.__file__}', f'--output={path.join(output, "licenses")}']
	gatherlicenses.main(len(gatherlicenses_args), gatherlicenses_args)

	if args.compress:
		zip_name = f"{output}.zip"
		z = ZipFile(zip_name, "w")
		for root, _, files in os.walk(output):
			for f in files:
				inner_path = path.relpath(path.join(root, f), output)
				print(f"{inner_path} -> {path.join(zip_name, inner_path)}")
				z.write(path.join(root, f), inner_path)
		z.close()


if __name__ == "__main__":
	main(len(sys.argv), sys.argv)
