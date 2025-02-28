下载到本地之后记得修改文件夹 config 中的 config.h 
需要修改的有IP和PORT，服务端和客户端都需要修改，不然在本地跑不起来
mysql相关不需要改，我们的mysql服务器是在云端的

新增Cjson第三方库,安装后才能编译通过
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON
mkdir build
cd build
cmake ..
make
sudo make install

安装后链接阶段加上-lcson