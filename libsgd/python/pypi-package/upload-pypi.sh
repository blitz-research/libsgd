# Upload package to pypi.
#
# Make sure to build-pypi.sh first.
#
# user/pass in $HOME/.pypirc
python -m twine upload --repository pypi --skip-existing dist/*
