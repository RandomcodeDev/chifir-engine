import os
import pefile
import sys

def generate_def_file(dll_path, def_path, name):
	pe = pefile.PE(dll_path)

	if not hasattr(pe, 'DIRECTORY_ENTRY_EXPORT'):
		print("The DLL does not contain any exports.")
		return

	with open(def_path, 'w') as def_file:
		def_file.write("LIBRARY " + name + "\n")
		def_file.write("EXPORTS\n")

		for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
			def_file.write(f"\t{exp.name.decode('utf-8') if exp.name else 'ord%d' % exp.ordinal} @{exp.ordinal}\n")

	print(f"DEF file {def_path} generated for {name} from {dll_path}")

if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("Usage: python mkdef.py <path_to_dll> <path_to_def> [name]")
	else:
		dll_path = sys.argv[1]
		def_path = sys.argv[2]
		name = ""
		if len(sys.argv) > 3:
			name = sys.argv[3]
		else:
			name = os.path.splitext(os.path.basename(dll_path))[0]
		generate_def_file(dll_path, def_path, name)

