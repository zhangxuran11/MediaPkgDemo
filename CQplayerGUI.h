#ifndef CQPLAYERGUI_H
#define CQPLAYERGUI_H
#include"CQipc.h"
#include"CQmedia.h"
#include"ui_CQplayerGUI.h"
class CQplayerGUI : public QDialog
{
    Q_OBJECT
    CQMedia* cqm;
    CQipc* cqi;

public:
    explicit CQplayerGUI(QWidget *parent = 0);

public:
    Ui::CQplayerGUI* ui;
private slots:
    void on_playMediaButton_clicked();
    void on_playIPCButton_clicked();
    void on_stopButton_clicked();
    void on_setOverButton_clicked();
};

#endif // CQPLAYERGUI_H
