cd ./build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja
cp compile_commands.json ../
cd ..