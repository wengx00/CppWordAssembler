# C++单词拼装器

华南师范大学编译原理课程实验一：C++单词拼装器

本应用的C++分词核心使用C++编写，GUI界面使用Electron + TypeScript + React编写。

## 1 编译环境

- CMake
- 操作系统（x86/arm均可）：
  - macOS：需要64位下的Xcode-Command-Line-Tools
  - Linux：需要64位下的GNU Make
  
  > 暂不支持Windows系统。
- Node.js 18.17+ 及 npm 9.0+



## 2 编译运行

> 本人是使用arm64结构下的macOS X 13.0/14.0 操作系统进行操作的。

首先需要在本地安装好[以上](#1 编译环境)列出的编译套件作为前提，完成后，以下详细描述如何在macOS系统下编译运行本项目。



### 2.1 安装依赖

进入源代码文件夹`Code`，在此打开终端，执行：

```shell
npm install
```

特别注意，安装的Electron版本必须是`v20.3.8`，也可以执行一次以下命令来确保安装的版本没有错误：

```shell
npm install -D electron@20.3.8
```

等待安装结束即可



### 2.2 编译方法

本项目的C++原生代码使用CMake + GNU Make来编译，首先介绍一下已经提前编写好的集成编译Shell，一次性编译并打包构建成可执行GUI应用，同样在终端执行：

```shell
sudo npm run build:all
```

> 在非macOS环境下无法使用该shell，请按下面的编译步骤拆解进行尝试。

正常情况下，稍等片刻即可看到：

- `core/build`目录下多了一个编译出的C++动态库：`libassembler.dylib` 
- `out`目录下构建出的GUI代码
- `dist`目录下编译构建出的可执行程序和安装包

至此编译构建完成。

---

如果是Linux环境，或遇到错误，请按照以下编译步骤进行尝试：

1. 编译C++代码成动态链接库（macOS下为`.dylib`，Linux下为`.so`）

   打开`Code/core`目录，在此打开终端，执行：

   ```shell
   cmake .
   ```
   执行后会根据CMakeLists.txt生成`Makefile`
   
   继续执行：
   
   ```shell
   cmake --build .
   ```

2. 根据编译出的`.dylib`/`.so`动态库文件名，修改`Code/src/preload/index.ts`下的nativePath路径，只需要将`libassembler.dylib`这个名称全局替换成实际编译出的动态库名称即可。

   ```ts
   const nativePath = 
     process.env.NODE_ENV === 'development' ? 
     path.resolve(__dirname, "../../core/build/libassembler.dylib") : // 第一处替换
     path.resolve(__dirname, "../../../native/libassembler.dylib") // 第二处替换
   ```

3. 根据编译出的动态库，修改构建配置`Code/electron-builder.yml`中的路径

   将`extraResources`配置下的from路径修改为正确的输出路径：

   ```yaml
   extraResources:
     - from: ./core/build # 改为动态库输出位置
       to: native
   ```

   

4. 将原生调用模块编译成兼容对应操作系统和Electron版本：

   返回到`Code`根目录下执行：

   ```shell
   sudo npm run rebuild
   ```

5. 根据对应操作系统构建Electron应用

   - macOS下执行：

     ```shell
     sudo npm run build:mac
     ```

   - Linux下执行：

     ```shell
     sudo npm run build:linux
     ```



### 2.3 运行程序

完成编译后，进入`Code/dist/mac-arm64`目录（Linux下选择对应目录），即可看到`C++单词拼装器.app`（或Linux可执行程序），双击即可运行。



