
### macOS arm64架构下的gstreamer学习教程
### 如何编译
在macOS上安装arm64的homebrew, 然后安装arm64位的gstreamer
```
cd /opt/homebrew/bin
./brew install gstreamer
```
这个要重点注意，生产环境的GStreamer.framework和devel的GStreamer.framework目录结构和内容是不同的

### 工程结构
src: 该目录下是一些官方的示例代码
### 一些参考文档
1. 官方文档 https://gstreamer.freedesktop.org/documentation/tutorials/index.html?gi-language=c

2. Gstreamer Github 页面 https://github.com/GStreamer

3. Gstreamer 会议视频 https://gstconf.ubicast.tv/

4. PyGObject 文档 https://lazka.github.io/pgi-docs/#Gst-1.0

5. Gstreamer 管道分析工具 (GstShark) http://developer.ridgerun.com/wiki/index.php?title=GstShark

6. awesome-gstreamer https://github.com/jackersson/awesome-gstreamer/blob/master/README.md

7. 使用 Gstreamer 的 5 个最酷的项目 https://lifestyletransfer.com/top-five-coolest-projects-with-gstreamer/

8. GStreamer 相关音视频技术
https://blog.csdn.net/automoblie0/article/details/146603037

9. gstreamer CMake 编译
https://cloud.tencent.com/developer/ask/sof/107978377

10. LiveVideoStack Gstreamer中的视频处理与硬件加速
https://cloud.tencent.com/developer/article/2257505


## 运行环境
1. 操作系统: macOS 14.3.1, 架构: arm64, 芯片: Apple M1
2. 可执行程序和3rdparty下的库都是支持arm64架构, 需要使用arm64架构的macOS电脑运行