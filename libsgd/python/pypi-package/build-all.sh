# Build all pypi wheels for current OS

function build() {
	echo ""
	echo "Building wheel cp${1}..."
	# Note: can't use an alias or var here for some reason, python is freaking wild...
	~/AppData/Local/Programs/Python/Python${1}/python.exe -m build --wheel
}

rm -rf build
rm -rf dist
rm -rf libsgd.egg-info

build "38"
build "39"
build "310"
build "311"

