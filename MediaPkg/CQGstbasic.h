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
        TIMEOUT,
        FATAL,
        QOS,
        WARNING,
        ERROR,
        STOP
    };
    enum PlayingState
    {
        PLAYING,
        PAUSED,
        STOPPED
    };
    CQGstBasic(int windowID,QWidget* win = 0 ,QObject *parent = NULL);
    ~CQGstBasic();



    PlayingState getPlayingState(){return state;}
    void printDot(const gchar* fileName);//打印dot图
    QString getUrl()const{return url;}
    virtual bool loadURL(const QString& url) = 0;
    void qosAdd(){qos_cnt++;}
    int getQosCnt()const{return qos_cnt;}
    void qosClear(){qos_cnt = 0;}
signals:
    void error(CQGstBasic::ErrorType);
protected:

    GstElement* pipeline;
    PlayingState state;

    QString url;
private:
    int qos_cnt;
    virtual void _updateDecoder() = 0;
    virtual void _updateDemux() = 0;
    const int winID;
    QWidget* barrier;

private slots:

    void _onOverlay();
    void _offOverlay();
public slots:

    bool paused();
    bool play();
    bool play(const QString& url);
    bool stop();
    void emitError(ErrorType type){emit error(type);}

};

#endif // CQGSTBASIC_H
