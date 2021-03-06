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
    case GST_MESSAGE_QOS:
        gstBasic->qosAdd();
        if(gstBasic->getQosCnt() < 10)
            break;
        gstBasic->qosClear();
        gstBasic->stop();
        gstBasic->emitError(CQGstBasic::QOS);
        qDebug("xxxxxxxxxxxxxxxxxxxxxxxxxxxxx send qos");
        //gstBasic->play(gstBasic->getUrl());
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
           gstBasic->emitError(CQGstBasic::WARNING);
           //gstBasic->play(gstBasic->getUrl());
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
        gstBasic->emitError(CQGstBasic::ERROR);
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
            gstBasic->emitError(CQGstBasic::TIMEOUT);
            qDebug("xxxxxxxxxxxxxxxxxxxxxxxxxxxxx send timeout");
            gst_bus_set_flushing(bus,true);

        }
        break;
    }
    default:;
    }
    return true;
}

void pad_added_handler (GstElement *, GstPad *new_pad, GstElement *elem)
{
    GstPad *sink_pad = NULL;
     GstPadLinkReturn ret;
     GstCaps *new_pad_caps = NULL;
     GstStructure *new_pad_struct = NULL;
     const gchar *new_pad_type = NULL;
    gchar *pad_name,*elem_name;
    pad_name = gst_pad_get_name (new_pad);
    elem_name = gst_element_get_name(elem);
    g_print ("A new pad %s was created for %s\n", pad_name,elem_name);
    if(!strncmp(pad_name,"audio",5) && !strncmp(elem_name,"queue2",6) )
    {
        g_print ("new_pad don't match the elem");
        goto exit;
    }
    if(!strncmp(pad_name,"video",5) && !strncmp(elem_name,"queue3",6) )
    {
        g_print ("new_pad don't match the elem");
        goto exit;
    }
    sink_pad = gst_element_get_static_pad (elem, "sink");

     g_print ("Received new pad '%s' for '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (elem));

     /* If our elem is already linked, we have nothing to do here */
     if (gst_pad_is_linked (sink_pad)) {
       g_print ("  We are already linked. Ignoring.\n");
       goto exit;
     }

     /* Check the new pad's type */
     new_pad_caps = gst_pad_get_caps (new_pad);
     new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
     new_pad_type = gst_structure_get_name (new_pad_struct);
//     if ( !g_str_has_prefix (new_pad_type, "audio/x-raw")) {
//       g_print ("  It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
//       goto exit;
//     }
     /* Attempt the link */
      ret = gst_pad_link (new_pad, sink_pad);
      g_print ("  err ret '%d' \n", ret);
      if (GST_PAD_LINK_FAILED (ret)) {
        g_print ("  Type is '%s' but link failed.\n", new_pad_type);
      } else {
        g_print ("  Link succeeded (type '%s').\n", new_pad_type);
      }

    sink_pad = gst_element_get_static_pad (elem, "sink");
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
      if(sink_pad != NULL)
          gst_object_unref(sink_pad);
      g_free (pad_name);
      g_free (elem_name);

      /* Unreference the new pad's caps, if we got them */
//      if (new_pad_caps != NULL)
//        gst_caps_unref (new_pad_caps);

      /* Unreference the sink pad */
//      gst_object_unref (sink_pad);
}
