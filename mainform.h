#ifndef MAINFORM_H
#define MAINFORM_H

#include <QMainWindow>
#include "QThread"
#include "QtCore"
#include "QCLoader.h"
#include "QtSerialPort/QSerialPort"
#include "QtSerialPort/QSerialPortInfo"
#include "QFileDialog"
QT_BEGIN_NAMESPACE
namespace Ui { class MainForm; }
QT_END_NAMESPACE


class MainThread : public QThread
{
  Q_OBJECT

public:
    MainThread(QObject *parent = 0);
    ~MainThread(){};
    void run();
    bool ConnectDevice(DiagInfo&);
    void SetLibraryMode();
    bool ConnectPort(DiagInfo&);
    QString ReadIMEI(DiagInfo&,int);
    bool SendSPC(DiagInfo&,QString SPC);
    bool SetSIMDual(DiagInfo&,bool dual);
    bool BackupQCN(DiagInfo&);
    bool RestoreQCN(DiagInfo&);
    bool SyncEFS(DiagInfo&);
    bool RebootNormal(DiagInfo&);
    void Disconnect();
    void EnableQcnNvItemCallBacks(DiagInfo & info);
    void static qphoneNvToolHandlerCallBack(uint handle,ushort sid, ushort iNViD, ushort iNVToolFuncEnum, ushort iEvent, ushort iProgress);
    static MainThread *Instance();
    static MainThread *instance_;


signals:
    void log(QString,QString);
    void progressValue(int);
    void progressText(QString);
};

class MainForm : public QMainWindow
{
    Q_OBJECT

public:
    MainForm(QWidget *parent = nullptr);
    ~MainForm();

    QString getSTR(QString begin,QString end, QString source)
    {
        int startIndex = source.indexOf(begin)+begin.length();
        if(startIndex <= 0)return QString();
        int endIndex = source.indexOf(end,startIndex);
        if(endIndex <= 0)return QString();
        return source.mid(startIndex,endIndex - startIndex);
    }

public slots:
    void log(QString,QString);
    void progressText(QString);

private slots:
    void on_btn_read_qcn_clicked();

    void on_btn_write_qcn_clicked();

    void on_btn_scan_port_clicked();

private:
    MainThread *thread;
    Ui::MainForm *ui;
};
#endif // MAINFORM_H
