# Build all pypi wheels for current OS

function build() {

	echo ""
	echo "Building wheel for cpython 3.${1}..."

	platform=$(uname)

	if [[ $platform == "msys" || $platform == "cygwin" ]]; then

	  	# Note: You can't use an alias or var here in gitbash for some reason, python is freaking wild...
  		~/AppData/Local/Programs/Python/Python3${1}/python.exe -m build --wheel

  	elif [[ $platform == "Darwin" ]]; then

    	python3.${1} -m build --wheel

	elif [[ $unamestr == 'Linux' ]]; then

		echo "TODO"
		exit 1
	fi
}

rm -rf build
rm -rf dist
rm -rf libsgd.egg-info

build "8"
#build "9""
#build "10"
#build "11"
