#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>
#include <stdio.h>

// 定义信号处理函数
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
    GstPad *sinkpad;
    GstElement *decoder = (GstElement *)data;

    g_print("Dynamic pad created, linking demuxer/decoder\n");

    sinkpad = gst_element_get_static_pad(decoder, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
        g_main_loop_quit(loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);

        g_printerr("Error: %s\n", error->message);
        g_error_free(error);

        g_main_loop_quit(loop);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

// appsink 的新样本回调函数
static GstFlowReturn new_sample(GstAppSink *appsink, gpointer user_data) {
    GstSample *sample;
    GstBuffer *buffer;
    GstMapInfo map;
    FILE *file = (FILE *)user_data; // Get the file pointer from user data

    // 从 appsink 获取新样本
    sample = gst_app_sink_pull_sample(appsink);
    if (sample) {
        buffer = gst_sample_get_buffer(sample);
        if (buffer) {

            // 获取 PTS 信息
            GstClockTime pts = GST_BUFFER_PTS(buffer);
            if (GST_CLOCK_TIME_IS_VALID(pts)) {
                g_print("Received frame with PTS: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(pts));
            } else {
                g_print("Received frame with invalid PTS\n");
            }

            if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
                // 这里可以处理内存中的 YUV 数据
                g_print("Received a frame of size %zu bytes\n", map.size);
                if (file) {
                    fwrite(map.data, 1, map.size, file);
                }
                // 示例：打印前 10 个字节
                for (guint i = 0; i < 10 && i < map.size; i++) {
                    g_print("%02x ", map.data[i]);
                }
                g_print("\n");

                gst_buffer_unmap(buffer, &map);
            }
        }
        gst_sample_unref(sample);
    }

    return GST_FLOW_OK;
}

int main(int argc, char *argv[]) {
    GMainLoop *loop;
    GstElement *pipeline, *source, *demuxer, *decoder, *convert, *appsink;
    GstBus *bus;
    guint bus_watch_id;
    FILE *file;

    /* 初始化 GStreamer */
    gst_init(&argc, &argv);

    argc = 2;
    argv[1] = "/Users/jason/Desktop/test10.mp4";
    /* 检查输入参数 */
    if (argc != 2) {
        g_printerr("Usage: %s <input MP4 file>\n", argv[0]);
        return -1;
    }

    loop = g_main_loop_new(NULL, FALSE);

    // Open the file in main function
    file = fopen("./test_live.yuv", "wb");
    if (!file) {
        g_printerr("Failed to open file for writing.\n");
        return -1;
    }

    /* 创建 GStreamer 元素 */
    pipeline = gst_pipeline_new("mp4-to-yuv-pipeline");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "mp4-demuxer");
    decoder = gst_element_factory_make("avdec_h264", "h264-decoder");
    convert = gst_element_factory_make("videoconvert", "video-converter");
    appsink = gst_element_factory_make("appsink", "video-appsink");

    if (!pipeline || !source || !demuxer || !decoder || !convert || !appsink) {
        g_printerr("One element could not be created. Exiting.\n");
        fclose(file); // Close the file before exiting
        return -1;
    }

    // 创建 capsfilter 元素
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    if (!capsfilter) {
        g_printerr("One element could not be created. Exiting.\n");
        fclose(file); // Close the file before exiting
        return -1;
    }
    // 设置 capsfilter 的媒体能力，指定 YUV 格式，这里以 I420 为例
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "I420",
                                        NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    gst_caps_unref(caps);

    /* 设置元素属性 */
    g_object_set(G_OBJECT(source), "location", argv[1], NULL);
    g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, "sync", FALSE, NULL);

    /* 添加消息处理器 */
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
    gst_object_unref(bus);

    /* 将所有元素添加到管道中 */
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, decoder, convert, appsink, NULL);

    /* 链接元素 */
    if (!gst_element_link(source, demuxer)) {
        g_printerr("Elements source and demuxer could not be linked.\n");
        fclose(file); // Close the file before exiting
        return -1;
    }
    if (!gst_element_link_many(decoder, convert, appsink, NULL)) {
        g_printerr("Elements decoder, convert and appsink could not be linked.\n");
        fclose(file); // Close the file before exiting
        return -1;
    }

    /* 动态链接 demuxer 到 decoder */
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), decoder);

    /* 连接 appsink 的新样本信号 */
    g_signal_connect(appsink, "new-sample", G_CALLBACK(new_sample), file);

    /* 将管道设置为播放状态 */
    g_print("Now decoding: %s\n", argv[1]);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* 进入主循环 */
    g_print("Running...\n");
    g_main_loop_run(loop);

    /* 退出主循环后，清理资源 */
    g_print("Returned, stopping playback\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);

    g_print("Deleting pipeline\n");
    gst_object_unref(GST_OBJECT(pipeline));
    g_source_remove(bus_watch_id);
    g_main_loop_unref(loop);
    fclose(file); // Close the file

    return 0;
}

/*
小端模式：低地址存储低字节，高地址存储高字节。
0x0102 存储为：02 01
大端模式：低地址存储高字节，高地址存储低字节。
0x0102 存储为：01 02
*/