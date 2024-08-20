# Build pypi distribution
#
# You only need to do this if uploading to testpypi or pypi.
rm -rf dist
rm -rf build
python -m build
