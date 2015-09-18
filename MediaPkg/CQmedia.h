#ifndef CQMEDIA_H
#define CQMEDIA_H

#include <gstreamer-0.10/gst/gst.h>
#include "CQGstbasic.h"


class CQMedia :public CQGstBasic
{
    Q_OBJECT
public:
    CQMedia(int windowID,QObject *parent = NULL);

    void setVolume(double volume);
    double getVolume();
    bool loadURL(const QString& url);
private:
    double volume;



    void _updateDecoder();

public slots:
    void setSilence(bool toggle);

};

#endif // CQMEDIA_H
