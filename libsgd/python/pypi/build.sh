
rm -rf dist
rm -rf build

cp ../../../cmake-build-windows-x64-release/LIBSGD_INSTALL/python/sgd.py libsgd
cp ../../../cmake-build-windows-x64-release/LIBSGD_INSTALL/python/_sgd.pyd libsgd

python -m build
