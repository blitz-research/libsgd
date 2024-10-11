# Build all pypi wheels for current OS

function build() {

	echo ""
	echo "Building wheel for cpython 3.${1}..."

	rm -rf build
	rm -rf libsgd.egg-info

	if [[ $OSTYPE == "msys" || $OSTYPE == "cygwin" ]]; then

  		~/AppData/Local/Programs/Python/Python3${1}/python.exe -m build --wheel

  	elif [[ $(uname) == "Darwin" ]]; then

    	python3.${1} -m build --wheel

	elif [[ $OSTYPE == "linux-gnu" ]]; then

		/usr/bin/python3.${1} -m build --wheel

	fi
}

rm -rf dist

build "8"
build "9"
build "10"
build "11"
build "12"
build "13"
