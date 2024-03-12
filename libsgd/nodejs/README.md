# Node's make system
npm install -g node-gyp

# Node's addon api
npm install -g node-addon-api

# Generate libsgd_wrap.cxx from libsgd.i
SWIG -c++ -javascript -napi libsgd.i

# Generate build dir from binding.gyp
#
# WARNING! Make sure to fix absolute paths in this file!
node-gyp configure

# Build!
node-gyp build
