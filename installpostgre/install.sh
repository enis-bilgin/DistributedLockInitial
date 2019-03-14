#!/usr/bin/env bash

echo "Building required libraries"

# Client Side -- psql
apt install postgresql postgresql-contrib

# C++ Dependencies
apt-get install libpq-dev postgresql-server-dev-all

wget https://github.com/jtv/libpqxx/archive/6.3.3.tar.gz
tar -xvzf 6.3.3.tar.gz && cd libpqxx-6.3.3/
mkdir build_ && cd build_ && make -j `nproc` && make install
ldconfig
