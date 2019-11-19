#!/bin/bash
apt-package/install.sh g++
apt-package/install.sh cmake
apt-package/install.sh make
systemlib/install.sh pthread
git/install.sh https://github.com/rockerbacon/assertions-test.git v1.0
git/install.sh https://github.com/rockerbacon/cpp-command-line-interface.git 3a17e3c44ddeab18292a9b118012e5f11f0f85b8
