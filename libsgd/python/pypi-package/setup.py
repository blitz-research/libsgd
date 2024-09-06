from setuptools import setup
import sys
import os

if sys.platform == "win32":
	ext = "pyd"
else:
	ext = "so"

path = os.path.join("libs", "py3" + str(sys.version_info.minor), "_sgd."+ext)

setup(
	package_data = {"libsgd": [path]},
	has_ext_modules = lambda: True
)
