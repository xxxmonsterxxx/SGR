#!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"

#library path
export LD_LIBRARY_PATH="${HERE}/lib"

exec "${HERE}/SGR_test" "$@"
