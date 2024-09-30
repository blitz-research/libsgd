# Added by Mark, proxy _sgd module that loads the real _sgd.pyd depending on runtime python version.
#
# We don't really need this anymore as we're publishing a wheel per cpython version now so there's only _sgd.pyd,
# but it doesn't hurt so leave it for now.

import importlib.util
import sys
import os

if sys.platform == "win32":
    ext = "pyd"
else:
    ext = "so"

path = os.path.join(os.path.dirname(__file__), "libs", "py3" + str(sys.version_info.minor), "_sgd."+ext)

if not os.path.exists(path):
    raise ImportError("_sgd."+ext+" not found")

spec = importlib.util.spec_from_file_location("_sgd", path)
_sgd_native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(_sgd_native)

sys.modules[__name__].__dict__.update(_sgd_native.__dict__)
