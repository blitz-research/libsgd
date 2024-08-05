rm -rf dist
rm -rf build
cp ../sgd.py libsgd
cp ../_sgd.pyd libsgd
python -m build

