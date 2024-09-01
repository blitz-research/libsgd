from setuptools import setup
import sys
import os

path = os.path.join("libs", "py3" + str(sys.version_info.minor), "_sgd.pyd")

setup(
	package_data = {"libsgd": [path]},
	has_ext_modules = lambda: True
)
