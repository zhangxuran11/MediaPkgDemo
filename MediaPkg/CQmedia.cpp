#include "CQmedia.h"
#include"sdpsrcparser.h"
#include"MediaPkgCommon.h"
#include<QDebug>
#include<QWidget>
CQMedia::CQMedia(int winID, QObject* parent):
    CQGstBasic(winID,(new QWidget())->winId(),parent)
{
    this->pipeline=gst_pipeline_new( "pipeline" );
    GstElement *v_src =  gst_element_factory_make("udpsrc","v_src");
    GstElement *videoh264depay  =  gst_element_factory_make("rtph264depay","videoh264depay");
    GstElement *videoque0 = gst_element_factory_make("queue","videoque0");
    //GstElement *videoque1 = gst_element_factory_make("queue","videoque1");
    g_object_set(G_OBJECT(v_src),"timeout",guint64(1000000),NULL);
    g_object_set(G_OBJECT(videoque0),"max-size-time",(guint64)0,"max-size-buffers",(guint64)0,NULL);
    //g_object_set(G_OBJECT(videoque1),"max-size-time",(guint64)100000,"max-size-buffers",(guint64)100000,NULL);


    GstElement *audiocnv  =  gst_element_factory_make("audioconvert","audiocnv");
    ///////////////////////////////////////////音频通道////////////////////////////////
    GstElement *a_src =  gst_element_factory_make("udpsrc","a_src");
    GstElement *audiodepay  =  gst_element_factory_make("rtpmp4gdepay","audiodepay");
    GstElement *audiovol = gst_element_factory_make("volume","audiovol");
    GstElement *audiosink =  gst_element_factory_make("alsasink","audiosink");
    g_object_set(G_OBJECT(audiosink),"sync",false,NULL);
#ifdef ARM
    ///////////////////////////////////////////视频通道////////////////////////////////


    GstElement *videodecoder =  gst_element_factory_make("mfw_vpudecoder","videodecoder");
    GstElement *videosink =  gst_element_factory_make("imxv4l2sink","videosink");
    g_object_set(G_OBJECT(videodecoder),"parser",true,"dbkenable",false,"profiling",true,
                 "framedrop",false,"min-latency",true,"loopback",true,"fmt",(guint64)1,NULL);
    if(pipeline==NULL||v_src==NULL||videoh264depay==NULL
            ||videoque0==NULL||videodecoder==NULL||videosink==NULL)
    {
        qDebug("video create failed\n");
    }

    gst_bin_add_many (GST_BIN (pipeline), v_src,videoque0,videoh264depay,videodecoder,videosink,NULL);
    gst_element_link_many ( v_src,videoh264depay,videoque0,videodecoder,videosink,NULL);
    ///////////////////////////////////////////音频通道////////////////////////////////
    GstElement *audiodecoder  =  gst_element_factory_make("beepdec","audiodecoder");
    g_object_set(G_OBJECT(videosink),"sync",false,"x11enable",true,NULL);
#endif

#ifdef  X86
    ///////////////////////////////////////////视频通道////////////////////////////////
    GstElement *videodecoder =  gst_element_factory_make("ffdec_h264","videodecoder");
    GstElement *colorspace =  gst_element_factory_make("ffmpegcolorspace","colorspace");
    GstElement *videosink =  gst_element_factory_make("ximagesink","videosink");
    if(pipeline==NULL||v_src==NULL||videoh264depay==NULL
            ||videoque0==NULL||videodecoder==NULL||colorspace==NULL||videosink==NULL)
    {
        qDebug("video create failed\n");
    }
    gst_bin_add_many (GST_BIN (pipeline), v_src, videoh264depay,videoque0,videodecoder,colorspace,videosink,NULL);
    gst_element_link_many ( v_src, videoh264depay,videoque0,videodecoder,colorspace,videosink,NULL);


    ///////////////////////////////////////////音频通道////////////////////////////////
    GstElement *audiodecoder  =  gst_element_factory_make("ffdec_aac","audiodecoder");
    g_object_set(G_OBJECT(videosink),"sync",false,NULL);
#endif

    ///////////////////////////////////////////音频通道////////////////////////////////
    if(a_src==NULL||audiodepay==NULL
            ||audiodecoder==NULL||audiocnv==NULL||audiovol==NULL||audiosink==NULL)
    {
        qDebug("audio create failed\n");
    }

    gst_bin_add_many (GST_BIN (pipeline), a_src,audiodepay,audiodecoder,audiocnv,audiovol,audiosink,NULL);
    gst_element_link_many ( a_src,audiodepay,audiodecoder,audiocnv,audiovol,audiosink,NULL);

    ///////////////////////////////////////////属性设置////////////////////////////////

        //添加消息监听函数
    GstBus* bus=gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus,bus_callback,this);
    gst_object_unref(bus);
    gst_element_set_state(pipeline,GST_STATE_NULL);
    this->state=STOPPED;

}
bool CQMedia::loadURL(const QString& url)
{

    this->url = url;
    SdpsrcParser sdpSrc(url);
    if(!sdpSrc.isValid())
    {
        qDebug()<<"SdpsrcParser invalid!!";
        return false;
    }
    sdpSrc.printf();
    GstElement *v_src =  gst_bin_get_by_name(GST_BIN(pipeline),"v_src");
    GstCaps *v_caps = gst_caps_new_simple("application/x-rtp","media",G_TYPE_STRING,"video",
                                          "payload",G_TYPE_INT,sdpSrc.v_payload,
                                      "clock-rate",G_TYPE_INT,sdpSrc.v_clock_rate,
                                  "encoding-name",G_TYPE_STRING,sdpSrc.v_coding.toUtf8().data(),
                                  "packetization-mode",G_TYPE_STRING,sdpSrc.v_packetization_mode.toUtf8().data(),
                                  "sprop-parameter-sets",G_TYPE_STRING,sdpSrc.v_sprop_parameter_sets.toUtf8().data(),
                                   "profile-level-id",G_TYPE_STRING,sdpSrc.v_profile_level_id.toUtf8().data(),
                                  NULL);
    QString uri = "udp://"+sdpSrc.broadcastIP+":"+QString::number(sdpSrc.v_port);
    g_object_set(G_OBJECT(v_src),"uri",uri.toUtf8().data(),"caps",v_caps,NULL);
    gst_object_unref(v_src);
    GstElement *a_src =  gst_bin_get_by_name(GST_BIN(pipeline),"a_src");
    uri = "udp://"+sdpSrc.broadcastIP+":"+QString::number(sdpSrc.a_port);
    GstCaps *a_caps = gst_caps_from_string(sdpSrc.get_a_caps_string());
    g_object_set(G_OBJECT(a_src),"uri",uri.toUtf8().data(),"caps",a_caps,NULL);
    gst_object_unref(a_src);

    return true;

}
void CQMedia::setVolume(double _volume)
{
    volume=_volume;
    GstElement *vol = gst_bin_get_by_name((GstBin *)pipeline,"audiovol");
    if(vol != NULL)
        g_object_set(vol,"volume",volume/10,NULL);
}
double CQMedia::getVolume()
{
    return this->volume;
}

void CQMedia::setSilence(bool toggle)
{
    if(toggle)
    {
        GstElement *vol = gst_bin_get_by_name((GstBin *)pipeline,"audiovol");
        if(vol != NULL)
            g_object_set(vol,"volume",0,NULL);
    }
    else
    {
        GstElement *vol = gst_bin_get_by_name((GstBin *)pipeline,"audiovol");
        if(vol != NULL)
            g_object_set(vol,"volume",volume/10,NULL);
    }
}

void CQMedia::_updateDecoder()
{
    GstElement *videodecoder = gst_bin_get_by_name((GstBin*)pipeline,"videodecoder");
    GstStateChangeReturn ret=gst_element_set_state(videodecoder,GST_STATE_NULL);
    if(GST_STATE_CHANGE_SUCCESS==ret)
    {
        qDebug("set videodecoder to null SUCCESS!!");
        if(gst_bin_remove(GST_BIN(pipeline),videodecoder))
        {
            qDebug("remove videodecoder success!!");
            gst_object_unref(videodecoder);

            videodecoder =  gst_element_factory_make("mfw_vpudecoder","videodecoder");
            g_object_set(G_OBJECT(videodecoder),"parser",true,"dbkenable",false,"profiling",true,
                         "framedrop",false,"min-latency",true,"loopback",true,"fmt",(guint64)1,NULL);

            GstElement *videoque0 = gst_bin_get_by_name((GstBin*)pipeline,"videoque0");
            GstElement *videosink = gst_bin_get_by_name((GstBin*)pipeline,"videosink");
            gst_bin_add_many (GST_BIN (pipeline),videodecoder,NULL);
            if(gst_element_link_many ( videoque0,videodecoder,videosink,NULL))
            {
                qDebug("link videodecoder  SUCCESS!!");
            }
            else
            {
                qDebug("link videodecoder  FAILED!!");
            }

            gst_object_unref(videoque0);
            gst_object_unref(videosink);

        }
        else
        {
            qDebug("remove videodecoder failed!!");
        }
    }
    else
    {
        qDebug("set videodecoder to null FAILED!!");
    }
}
