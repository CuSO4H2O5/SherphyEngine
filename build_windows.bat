@echo off

cmake -S . -B build -DSherphy_CMAKE_DEBUG=1
cmake --build build --config Release 