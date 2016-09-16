#!/bin/sh
if [ "$1" == clean ]; then
    git clean -fdx
else
    autoreconf --force --install
fi
