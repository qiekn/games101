#!/bin/bash

shopt -s globstar
clang-format -i **/*.hpp **/*.cpp **/*.h
