#!/bin/bash

File_List=$(find src/ | grep -E '.+\.(cpp|hpp)' --color=never)

clang-format --verbose -i ${File_List}
