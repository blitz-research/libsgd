import re
import sys
from pathlib import Path

# Insane convert CapitalCase to snake_case, with hacks for seqs of caps like NX, CSM etc and 2D
# regex = r"(?<=[a-z])(?=[A-Z])|(?<=[A-Z][A-Z])(?=[A-Z][a-z])|(?<=2D)(?=[A-Z])|(?<=[a-z])(?=2D)";

def findRenames(inFile):

	with open(inFile, 'r') as file:
		text = file.read()

	renames = []

	# Functions, eg: SGD_API SGD_EventMask SGD_DECL sgd_PollEvents();
	#
	pattern = r'^\s*SGD_API\s+\w+\s+SGD_DECL\s+sgd_(\w+)'
	for match in re.finditer(pattern, text, flags=re.MULTILINE):
		ident = match.group(1)  # 'ident with sgd_ prefix stripped'

		lcident = ident[:1].lower() + ident[1:]

		renames.append(f'%rename({lcident}) sgd_{ident};')

	# Typedefs, eg: typedef enum SGD_EventMask
	#
	pattern = r'^\s*typedef\s+\w+\s+SGD_(\w+)'
	for match in re.finditer(pattern, text, flags=re.MULTILINE):
		ident = match.group(1)

		renames.append(f'%rename({ident}) SGD_{ident};')

	# Enum members, eg: SGD_EVENT_MASK_CLOSE_CLICKED = 0x01
	#
	pattern = r'^\s*SGD_(\w+)\s+=\s+'
	for match in re.finditer(pattern, text, flags=re.MULTILINE):
		ident = match.group(1)

		renames.append(f'%rename({ident}) SGD_{ident};')

	return renames


if __name__ == "__main__":

	if len(sys.argv) != 3:
		print("Usage: python script.py incDir, outFile")
		sys.exit(1)

	incDir = sys.argv[1]

	renames = []

	renames.extend(findRenames(incDir + "/sgd/sgd.h"))
	renames.extend(findRenames(incDir + "/sgd/keycodes.h"))

	with open(sys.argv[2], 'w') as file:
		file.write("\n".join(renames))
