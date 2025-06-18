#include "Decoder.h"

Decoder::Decoder() : loop(nullptr), pipeline(nullptr), source(nullptr), demuxer(nullptr),
                     decoder(nullptr), convert(nullptr), appsink(nullptr),
                     bus(nullptr), bus_watch_id(0), file(nullptr), is_playing(false) {}

Decoder::~Decoder() {
    stop();
    if (file) {
        fclose(file);
    }
}

// 静态成员函数调用实际的回调函数
void Decoder::on_pad_added_static(GstElement *element, GstPad *pad, gpointer data) {
    Decoder *decoder_obj = static_cast<Decoder*>(data);
    decoder_obj->on_pad_added(element, pad);
}

gboolean Decoder::bus_call_static(GstBus *bus, GstMessage *msg, gpointer data) {
    Decoder *decoder_obj = static_cast<Decoder*>(data);
    return decoder_obj->bus_call(bus, msg);
}

GstFlowReturn Decoder::new_sample_static(GstAppSink *appsink, gpointer user_data) {
    Decoder *decoder_obj = static_cast<Decoder*>(user_data);
    return decoder_obj->new_sample(appsink);
}

// 实际的回调函数
void Decoder::on_pad_added(GstElement *element, GstPad *pad) {
    GstPad *sinkpad = gst_element_get_static_pad(decoder, "sink");
    g_print("Dynamic pad created, linking demuxer/decoder\n");
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}

gboolean Decoder::bus_call(GstBus *bus, GstMessage *msg) {
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
        g_main_loop_quit(loop);
        is_playing = false;
        break;
    case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *error;
        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);
        g_printerr("Error: %s\n", error->message);
        g_error_free(error);
        g_main_loop_quit(loop);
        is_playing = false;
        break;
    }
    default:
        break;
    }
    return TRUE;
}

GstFlowReturn Decoder::new_sample(GstAppSink *appsink) {
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (sample) {
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        if (buffer) {

            // 获取 PTS 信息
            GstClockTime pts = GST_BUFFER_PTS(buffer);
            if (GST_CLOCK_TIME_IS_VALID(pts)) {
                g_print("Received frame with PTS: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(pts));
            } else {
                g_print("Received frame with invalid PTS\n");
            }
            
            GstMapInfo map;
            if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
                GstCaps *caps = gst_sample_get_caps(sample);
                if (caps) {
                    GstStructure *structure = gst_caps_get_structure(caps, 0);
                    if (structure) {
                        int width, height;
                        if (gst_structure_get_int(structure, "width", &width) &&
                            gst_structure_get_int(structure, "height", &height)) {
                            std::lock_guard<std::mutex> lock(frame_mutex);
                            latest_frame_data.assign(map.data, map.data + map.size);
                            latest_frame_width = width;
                            latest_frame_height = height;
                            new_frame_available = true;
                        }
                    }
                }
                g_print("Received a frame of size %zu bytes\n", map.size);
                if (file) {
                    fwrite(map.data, 1, map.size, file);
                }
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

bool Decoder::init(const char *input_file) {
    /* 初始化 GStreamer */
    gst_init(NULL, NULL);
    loop = g_main_loop_new(NULL, FALSE);

    /* 创建 GStreamer 元素 */
    pipeline = gst_pipeline_new("mp4-to-yuv-pipeline");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "mp4-demuxer");
    decoder = gst_element_factory_make("avdec_h264", "h264-decoder");
    convert = gst_element_factory_make("videoconvert", "video-converter");
    appsink = gst_element_factory_make("appsink", "video-appsink");

    if (!pipeline || !source || !demuxer || !decoder || !convert || !appsink) {
        g_printerr("One element could not be created. Exiting.\n");
        return false;
    }

    // 创建 capsfilter 元素
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    if (!capsfilter) {
        g_printerr("One element could not be created. Exiting.\n");
        return false;
    }
    // 设置 capsfilter 的媒体能力，指定 YUV 格式，这里以 I420 为例
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "I420",
                                        NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    gst_caps_unref(caps);

    /* 设置元素属性 */
    g_object_set(G_OBJECT(source), "location", input_file, NULL);
    g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, "sync", FALSE, NULL);

    /* 添加消息处理器 */
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch(bus, bus_call_static, this);
    gst_object_unref(bus);

    /* 将所有元素添加到管道中 */
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, decoder, convert, appsink, NULL);

    /* 链接元素 */
    if (!gst_element_link(source, demuxer)) {
        g_printerr("Elements source and demuxer could not be linked.\n");
        return false;
    }
    if (!gst_element_link_many(decoder, convert, appsink, NULL)) {
        g_printerr("Elements decoder, convert and appsink could not be linked.\n");
        return false;
    }

    /* 动态链接 demuxer 到 decoder */
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added_static), this);

    /* 连接 appsink 的新样本信号 */
    g_signal_connect(appsink, "new-sample", G_CALLBACK(new_sample_static), this);

    /* 将管道设置为播放状态 */
    g_print("Now decoding: %s\n", input_file);
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        return false;
    }
    is_playing = true;

    return true;
}

bool Decoder::DecodeOnFrame(std::vector<uint8_t> &data, int &width, int &height) {
    if (!is_playing) {
        return false;
    }
    // 处理一帧事件
    g_main_context_iteration(g_main_loop_get_context(loop), TRUE);

    std::lock_guard<std::mutex> lock(frame_mutex);
    if (new_frame_available) {
        data = latest_frame_data;
        width = latest_frame_width;
        height = latest_frame_height;
        new_frame_available = false;
        return true;
    }
    return false;
}

void Decoder::stop() {
    if (is_playing) {
        g_print("Stopping playback\n");
        gst_element_set_state(pipeline, GST_STATE_NULL);
        g_print("Deleting pipeline\n");
        if (pipeline) {
            gst_object_unref(GST_OBJECT(pipeline));
            pipeline = nullptr;
        }
        if (bus_watch_id) {
            g_source_remove(bus_watch_id);
            bus_watch_id = 0;
        }
        if (loop) {
            g_main_loop_unref(loop);
            loop = nullptr;
        }
        is_playing = false;
    }
}