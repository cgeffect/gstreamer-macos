#ifndef DECODER_H
#define DECODER_H

#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>
#include <stdio.h>
#include <vector>
#include <mutex>

class Decoder {
private:
    GMainLoop *loop;
    GstElement *pipeline, *source, *demuxer, *decoder, *convert, *appsink;
    GstBus *bus;
    guint bus_watch_id;
    FILE *file;
    bool is_playing;
    std::vector<uint8_t> latest_frame_data;
    int latest_frame_width = 0;
    int latest_frame_height = 0;
    std::mutex frame_mutex;
    bool new_frame_available = false;

    // 静态成员函数作为回调
    static void on_pad_added_static(GstElement *element, GstPad *pad, gpointer data);
    static gboolean bus_call_static(GstBus *bus, GstMessage *msg, gpointer data);
    static GstFlowReturn new_sample_static(GstAppSink *appsink, gpointer user_data);

    // 实际的回调函数
    void on_pad_added(GstElement *element, GstPad *pad);
    gboolean bus_call(GstBus *bus, GstMessage *msg);
    GstFlowReturn new_sample(GstAppSink *appsink);

public:
    Decoder();
    ~Decoder();
    bool init(const char *input_file);
    bool DecodeOnFrame(std::vector<uint8_t> &data, int &width, int &height);
    void stop();
};

#endif // DECODER_H