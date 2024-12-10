import os
import pefile
import sys

def generate_def_file(dll_path, def_path, name):
	pe = pefile.PE(dll_path)

	if not hasattr(pe, 'DIRECTORY_ENTRY_EXPORT'):
		print("The DLL does not contain any exports.")
		return

	with open(def_path, 'wb') as def_file:
		def_file.write("LIBRARY " + name + "\n")
		def_file.write("EXPORTS\n")

		for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
			def_file.write(f"\t{exp.name.decode('utf-8') if exp.name else 'ord%d' % exp.ordinal} @{exp.ordinal}\n")

	print(f"DEF file {def_path} generated for {name} from {dll_path}")

def main(argc, argv):
	if argc < 3:
		print(f"Usage: {argv[0]} <.dll file> <.def file> [lib name]")
	else:
		dll_path = argv[1]
		def_path = argv[2]
		name = ""
		if argc > 3:
			name = argv[3]
		else:
			name = os.path.splitext(os.path.basename(dll_path))[0]
		generate_def_file(dll_path, def_path, name)

if __name__ == "__main__":
	main(len(sys.argv), sys.argv)
