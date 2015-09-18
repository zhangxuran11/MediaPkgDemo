#ifndef CQGSTBASIC_H
#define CQGSTBASIC_H

#include <QObject>
#include <gstreamer-0.10/gst/gst.h>
class CQGstBasic :public QObject
{
    Q_OBJECT

public:
    enum ErrorType
    {
        Fatal
    };
    enum PlayingState
    {
        PLAYING,
        PAUSED,
        STOPPED
    };
    CQGstBasic(int windowID,int windowID_null,QObject *parent = NULL);
    ~CQGstBasic();



    PlayingState getPlayingState(){return state;}
    void printDot(const gchar* fileName);//打印dot图
    QString getUrl()const{return url;}
    virtual bool loadURL(const QString& url) = 0;
signals:
    void error(ErrorType);
protected:
    GstElement* pipeline;
    PlayingState state;

    QString url;
private:

    virtual void _updateDecoder() = 0;
    const int winID;
    const int winID_null;
private slots:

    void _onOverlay();
    void _offOverlay();
public slots:

    bool paused();
    bool play();
    bool play(const QString& url);
    bool stop();

};

#endif // CQGSTBASIC_H