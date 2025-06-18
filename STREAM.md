ZLMediaKit搭建服务器

ffmpeg推送rtmp到本地的服务器
ffmpeg -re -i input.mp4 -c:v libx264 -preset medium -tune zerolatency -b:v 1000k -c:a aac -b:a 128k -f flv rtmp://localhost/live/stream

gstreamer播放rtmp
gst-launch-1.0 playbin uri=rtmp://localhost/live/stream

gstreamer搭建rtsp服务器(貌似不再更新了)
https://github.com/GStreamer/gst-rtsp-server

新的rtsp-server在 https://gitlab.freedesktop.org/gstreamer/gstreamer/-/tree/main/subprojects/gst-rtsp-server?ref_type=heads
