from setuptools import setup
import sys
import os

if sys.platform == "win32":
	ext = "pyd"
elif sys.platform == "darwin":
	ext = "so"
elif sys.platform == "linux":
	ext = "so"
else:
	raise RuntimeError("Unrecognized sys.plaform '"+sys.platform+"'")

path = os.path.join("libs", "py3" + str(sys.version_info.minor), "_sgd."+ext)

setup(
	package_data = {"libsgd": [path]},
	has_ext_modules = lambda: True
)
