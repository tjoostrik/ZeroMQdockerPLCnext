# ZeroMQdocker
With this repository you can create a docker image for a ELF32 ARM based Hard Float machine such as the PLCnext.

# Requirements
cmake version 2.6 minimum
PLCnext SDK installed

# Installation of docker
## Linux
source /directory to sdk

example: source /opt/pxc/sdk/AXCF2152/2021.0.1/environment-setup-cortexa9t2hf-neon-pxc-linux-gnueabi

git clone https://github.com/tjoostrik/zeromqdockerplcnext
cd zeromqdockerplcnext
cmake .
make
docker buildx build --platform linux/arm/v7 .
docker run {imageId}

To use the image you can look at the conf file
You can edit the conf file and add it to the docker using -v path/to/conf/file:/Configfile/
example: docker run f9afefa6acca -v /home/user/Download/zeromqdockerplcnext/Configfile/:/Configfile/
