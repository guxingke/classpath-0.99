# Classpath 0.99

为 JamVM macos 版本的 classpath 支持项目。

## 编译

### 快速编译
```bash
# 安装 antlr
brew install antlr

# 根目录不可写，重新挂载一下
sudo mount -uw /

# 项目路径
sudo mkdir -p /usr/local/app
sudo chown -R `whoami`:admin /usr/local/app

# 安装路径
sudo mkdir -p /usr/local/classpath
sudo chown -R `whoami`:admin /usr/local/classpath

# 下载并解压源文件
cd /usr/local/app
git clone git@github.com:guxingke/classpath-0.99.git classpath

# 编译
cd classpath
./configure --disable-plugin --disable-gtk-peer --disable-gjdoc --disable-examples --disable-gconf-peer --with-glibj=both
make install
```

### 从 GNU Classpath 0.99 源文件编译

- [在 macOS 上编译 GNU Classpath](https://www.guxingke.com/post/gnu-classpath-on-macos.html)

## 局限

1. 仅为 macos Catalina 支持。

