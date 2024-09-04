# Build all pypi wheels for current OS

function build() {

	echo ""
	echo "Building wheel for cpython 3.${1}..."

	platform=$(uname)

	if [[ $platform == "msys" || $platform == "cygwin" ]]; then

  		~/AppData/Local/Programs/Python/Python3${1}/python.exe -m build --wheel

  	elif [[ $platform == "Darwin" ]]; then

    	python3.${1} -m build --wheel

	elif [[ $platform == 'Linux' ]]; then

		/usr/bin/python3.${1} -m build --wheel

	fi
}

rm -rf build
rm -rf dist
rm -rf libsgd.egg-info

build "8"
#build "9""
#build "10"
#build "11"
