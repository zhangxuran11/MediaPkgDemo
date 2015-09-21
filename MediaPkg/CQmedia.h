#ifndef CQMEDIA_H
#define CQMEDIA_H

#include <gstreamer-0.10/gst/gst.h>
#include "CQGstbasic.h"

class CQMedia :public CQGstBasic
{
    Q_OBJECT
public:
    CQMedia(int windowID,QWidget *win,QObject *parent = NULL);

    void setVolume(double volume);
    double getVolume();
    bool loadURL(const QString& url);
    void setSilence(bool toggle);
    bool isSilence()const{return silenceState;}
private:
    double volume;
    bool silenceState;


    void _updateDecoder();

public slots:


};

#endif // CQMEDIA_H
