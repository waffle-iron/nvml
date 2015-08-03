#!/bin/bash
set -e
if [ ! -d $HOME/valgrind/bin ]; then
    wget https://github.com/tomaszkapela/valgrind/archive/v0.1-alpha.1.tar.gz
    tar -xzf v0.1-alpha.1.tar.gz;
    cd valgrind-0.1-alpha.1 && ./autogen.sh &&
    ./configure --prefix=$HOME/valgrind && make -j && make install;
    cd .. && rm -rf valgrind*;
else
    echo "Using cached directory"
fi
