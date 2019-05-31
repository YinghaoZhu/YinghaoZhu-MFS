# YinghaoZhu-MFS

# 关于mfs   
mfs是一个简单的基于fuse开发的内存文件系统    
需要在linux下进行编译和测试，建议使用ubuntu系统    
用户可以使用mount挂载mfs，使用umount卸载mfs    

# 安装必要的软件  

安装pkg-config  
$ sudo apt install pkg-config  
安装libfuse-dev  
$ sudo apt install libfuse-dev  

# 编译
进入mfs目录  
$ cd mfs  
编译  
$ make  
测试和使用  

# 建立挂载点
$ mkdir /tmp/test   
将mfs系统挂载到/tmp/test  
$ ./mfs -v /tmp/test   
使用文件浏览器查看/tmp/test  
# 卸载/tmp/test 
$ fusermount -u /tmp/test  
