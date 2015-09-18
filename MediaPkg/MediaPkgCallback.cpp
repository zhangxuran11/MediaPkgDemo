#include <gstreamer-0.10/gst/gst.h>
#include"CQGstbasic.h"
#include<QDebug>
#include"ztools.h"
gboolean bus_callback(GstBus *bus,GstMessage *msg,gpointer data)
{
    CQGstBasic *gstBasic=(CQGstBasic*)data;
    qDebug("%s recive message:%s\n",gst_object_get_name (GST_MESSAGE_SRC(msg)),GST_MESSAGE_TYPE_NAME(msg));
    switch(GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
    break;
    case GST_MESSAGE_WARNING:
    {
        GError *err;
        gchar *debug;
        gst_message_parse_warning (msg, &err, &debug);
        g_print ("Error: %s\n", err->message);
        if(!strcmp(err->message,"Could not decode stream."))
        {
           gstBasic->stop();
           gstBasic->play(gstBasic->getUrl());
        }

        g_error_free (err);
        g_free (debug);
        break;
    }
    case GST_MESSAGE_ERROR:
    {
        GError *err;
        gchar *debug;
        gst_message_parse_error (msg, &err, &debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
        g_free (debug);

        gstBasic->stop();
        gstBasic->play(gstBasic->getUrl());
        qDebug("Error happened\n");

        break;
    }

    case GST_MESSAGE_ELEMENT:
    {
        const GstStructure *structure;
        structure = gst_message_get_structure (msg);
        qDebug()<<(char*)gst_structure_to_string (structure);
        qDebug()<<(char*)gst_structure_get_name (structure);
        if(!strcmp(gst_structure_get_name (structure),"GstUDPSrcTimeout"))
        {
            gstBasic->stop();
            gst_bus_set_flushing(bus,true);
            ZTools::msleep(3000);
            gstBasic->play(gstBasic->getUrl());
        }
        break;
    }
    default:;
    }
    return true;
}

void pad_added_handler (GstElement *, GstPad *new_pad, GstElement *elem)
{
    GstPad *sink_pad = gst_element_get_static_pad (elem, "sink");
    GstPadLinkReturn ret;
//      /* If our converter is already linked, we have nothing to do here */
      if (gst_pad_is_linked (sink_pad)) {
        g_print ("  We are already linked. Ignoring.\n");
        goto exit;
      }
      /* Attempt the link */
      ret = gst_pad_link (new_pad, sink_pad);
      if (GST_PAD_LINK_FAILED (ret)) {
        g_print ("  link failed.\n");
      } else {
        g_print ("  Link succeeded.\n");
      }

    exit:
      /* Unreference the sink pad */
      gst_object_unref (sink_pad);
}
