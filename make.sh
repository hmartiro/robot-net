mkdir -p build &&
cd build &&
cmake ..  &&
time make &&
#cp -rf iron_dome ../ &&
cd ..
