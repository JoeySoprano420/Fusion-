@echo off
echo Downloading nlohmann/json.hpp...

mkdir include
curl -L https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o include\json.hpp

echo json.hpp saved to /include
echo Use with: #include "include/json.hpp"
pause
