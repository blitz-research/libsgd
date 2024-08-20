# Alas, SWIG's rename mechanism isn't quite sophisticated enough to handle stripping SGD_|sgd_ prefix AND converting the
# reset to snake case, which is where this beast comes in, written mostly by my chatGPT 'Buddy'!
#
import re
from pathlib import Path
import sys


def extract_and_convert_identifiers(text):
    """
    Extract identifiers after 'SGD_DECL' that start with 'sgd_' or 'SGD_',
    convert them to snake_case, and generate SWIG %rename statements.
    """
    rename_statements = []

    # Regex pattern to match identifiers after 'SGD_DECL' that start with 'sgd_' or 'SGD_'
    pattern = re.compile(r'(SGD_DECL|#define)\s+(sgd_|SGD_)([A-Za-z_][A-Za-z0-9_]*)')

    for match in pattern.finditer(text):
        decl = match.group(1)  # 'SGD_DECL' or '#define'
        prefix = match.group(2)  # The 'sgd_' or 'SGD_' prefix
        identifier = match.group(3)  # The rest of the identifier

        if identifier == "SGD_H_INCLUDED" or identifier == "EXTERN" or identifier == "API" or identifier == "DECL":
            continue

        output_name = identifier

        if decl == "SGD_DECL":
            # Convert CamelCase to snake_case, with hacks for seqs of caps like NX, CSM etc and 2D
            regex = r"(?<=[a-z])(?=[A-Z])|(?<=[A-Z][A-Z])(?=[A-Z][a-z])|(?<=2D)(?=[A-Z])|(?<=[a-z])(?=2D)";

            #            snake_case_name = re.sub(r'(?<=[a-z])(?=[A-Z])|(?<=[^a-z][A-Z])(?=[A-Z][a-z])|(?=2D)', '_', identifier).lower()
            snake_case_name = re.sub(regex, '_', identifier).lower()

            # Remove the prefix for the output name
            output_name = snake_case_name

        # Create the %rename statement
        rename_statement = f'%rename({output_name}) {prefix + identifier};'
        rename_statements.append(rename_statement)

    return rename_statements


def process_files(input_file, output_file):
    with open(input_file, 'r') as file:
        text = file.read()

    rename_statements = extract_and_convert_identifiers(text)

    output_path = Path(output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    with output_path.open('w') as file:
        file.write('\n'.join(rename_statements))

    print(f"Generated SWIG %rename statements and saved to {output_file}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    process_files(input_file, output_file)
