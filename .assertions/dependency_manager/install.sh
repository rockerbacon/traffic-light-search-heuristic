#!/bin/bash
source apt-package/install.sh g++
source apt-package/install.sh cmake
source apt-package/install.sh make
source git/install.sh rockerbacon/cpp-command-line-interface --version 3a17e3c44ddeab18292a9b118012e5f11f0f85b8
source git/install.sh rockerbacon/assertions-test --version v3.1
source git/install.sh rockerbacon/cpp-benchmark --version v1.0.6
source git/install.sh rockerbacon/stopwatch --version v2.1.4
