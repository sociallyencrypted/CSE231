cp -r ./linux-5.19.8 ./1/1.2/kernelbuild/A/linux-5.19.8

cp -r ./1/1.2/.config ./1/1.2/kernelbuild/A/linux-5.19.8/.config

cd ./1/1.2/kernelbuild/A/linux-5.19.8

make -j$(nproc)

echo "Build A done"