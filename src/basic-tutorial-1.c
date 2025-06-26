#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

/**
 * GStreamer基础教程1 - 简单的媒体播放器
 * 这个示例展示了如何使用GStreamer创建一个基本的媒体播放器
 * 使用playbin元素来播放网络或本地媒体文件
 */
int tutorial_main(int argc, char *argv[]) {
    GstElement *pipeline;    // GStreamer管道元素，用于管理整个媒体处理流程
    GstBus *bus;             // 消息总线，用于接收管道状态变化和错误消息
    GstMessage *msg;         // 消息对象，用于处理从总线接收到的消息

    /* 初始化GStreamer库 */
    // 这个函数必须在使用任何GStreamer功能之前调用
    // 它会初始化GStreamer的内部数据结构、注册插件等
    gst_init(&argc, &argv);

    /* 构建媒体播放管道 */
    // gst_parse_launch() 函数可以根据字符串描述创建管道
    // "playbin" 是一个高级元素，它内部包含了完整的播放管道
    // 它会自动处理解复用、解码、音频/视频同步等复杂操作
    // uri参数指定要播放的媒体文件地址（可以是网络URL或本地文件路径）
    pipeline = gst_parse_launch("playbin uri=https://mogic-effect-test.oss-cn-hangzhou.aliyuncs.com/test/webm/sintel_trailer-480p.webm", NULL);
    
    // 备用选项：播放本地文件
    // pipeline = gst_parse_launch("playbin uri=file:///Users/jason/Desktop/sintel_trailer-480p.webm", NULL);

    /* 开始播放 */
    // 将管道状态设置为PLAYING，开始媒体播放
    // GST_STATE_PLAYING 表示管道正在播放媒体
    GstStateChangeReturn state = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    // 检查状态转换是否成功
    if (state == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);  // 释放管道资源
        return -1;
    }

    /* 等待播放结束或发生错误 */
    // 获取管道的消息总线，用于监听管道事件
    bus = gst_element_get_bus(pipeline);
    
    // 阻塞等待消息，直到收到错误消息或播放结束消息
    // GST_CLOCK_TIME_NONE 表示无限等待
    // GST_MESSAGE_ERROR | GST_MESSAGE_EOS 表示只监听错误和播放结束消息
    // EOS = End Of Stream，表示媒体流播放完毕
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* 清理资源 */
    // 如果收到了消息，释放消息对象
    if (msg != NULL)
        gst_message_unref(msg);
    
    // 释放总线对象
    gst_object_unref(bus);
    
    // 将管道状态设置为NULL，停止所有处理
    gst_element_set_state(pipeline, GST_STATE_NULL);
    
    // 释放管道对象及其所有子元素
    gst_object_unref(pipeline);
    
    return 0;
}

/**
 * 主函数 - 处理不同平台的差异
 * 在macOS上，需要使用特殊的main函数来正确处理GUI事件循环
 */
int main(int argc, char *argv[]) {
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
    // macOS平台：使用gst_macos_main来正确处理Cocoa事件循环
    // 这确保了GStreamer能够与macOS的GUI系统正确集成
    return gst_macos_main((GstMainFunc)tutorial_main, argc, argv, NULL);
#else
    // 其他平台：直接调用tutorial_main函数
    return tutorial_main(argc, argv);
#endif
}