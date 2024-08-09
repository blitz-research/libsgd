#
# Added by Mark, proxy _sgd module that loads the real _sgd.pyd depending on runtime python version.
#
import importlib.util
import sys
import os

path = os.path.join(os.path.dirname(__file__), "libs", "py3" + str(sys.version_info.minor), "_sgd.pyd")

if not os.path.exists(path):
    raise ImportError("_sgd.pyd not found")

spec = importlib.util.spec_from_file_location("_sgd", path)
_sgd_native = importlib.util.module_from_spec(spec)
spec.loader.exec_module(_sgd_native)

sys.modules[__name__].__dict__.update(_sgd_native.__dict__)
