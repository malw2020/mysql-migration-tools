#!/bin/bash

res=0;

dir=`pwd`
echo $dir

cd $dir && tar xvzf $dir/log4cpp-1.1.tar.gz -C . && cd $dir/log4cpp && ./configure --prefix=/usr/local && make && make install
res=$?
if (( res != 0 ));then
    echo "Install log4cpp failed."
    exit 1;
fi

