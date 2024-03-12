# Node's make system
npm install -g node-gyp

# Node's addon api
npm install -g node-addon-api

# Generate build dir from binding.gyp
#
# WARNING! Make sure to fix absolute paths in this file!
node-gyp configure
