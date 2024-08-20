# Upload package to testpypi.
#
# Make sure to build-pypi.sh first.
#
# user/pass in $HOME/.pypirc
python -m twine upload --repository testpypi --skip-existing dist/*
