#!/bin/bash

# git submodule update --init --recursive

# Xcode 默认找/usr/local/bin/node下的node，所以需要手动链接到/usr/local/bin/node, 使用下面的命令把node链接到/usr/local/bin/node
# 二选一, 都可以
# sudo ln -s /opt/homebrew/Cellar/node/23.9.0/bin/node /usr/local/bin/node
# sudo ln -s $(which node) /usr/local/bin/node

rm -rf Xcode && mkdir Xcode && cd Xcode
cmake -G Xcode ..
# cmake -G Xcode -DBUILD_USE_XCODE=ON ..

echo "========================================="
echo "build xcode success"
echo "========================================="
