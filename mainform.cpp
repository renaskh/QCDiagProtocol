#include "mainform.h"
#include "ui_mainform.h"

enum Action
{
    ReadQCN,
    WriteQCN
} action;
int cport;
QString qpath;

MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainForm)
{
    ui->setupUi(this);
    thread = new MainThread(this);
    connect(thread, SIGNAL(log(QString,QString)),this, SLOT(log(QString,QString)));
    connect(thread, SIGNAL(progressValue(int)),ui->progressBar,SLOT(setValue(int)));
    connect(thread, SIGNAL(progressText(QString)),this, SLOT(progressText(QString)));
    ui->progressBar->setAlignment(Qt::AlignCenter);
    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()){ ui->cb_comport->addItem(port.description()+" ("+port.portName()+")");}
}


MainForm::~MainForm()
{
    delete ui;
}

void MainThread::run()
{
    instance_ = this;
    DiagInfo info;
    info.portnum = cport;
    if (!ConnectDevice(info))
        return;

    if (action == ReadQCN)
    {
        log("\nReading QCN from phone...","black");
        int renas2;
        QDateTime dteNow = QDateTime::currentDateTime();
        QString fnl = dteNow.toString("smh_d_M_yyyy").replace(" ","_").replace(":","_");
        QString path = QDir().currentPath()+"/QCN_"+fnl+".qcn";

        if (!QLIB_BackupNVFromMobileToQCN(info.hndl,path.toLocal8Bit().data(),&renas2))
        {
            log(" error","red");
            return;
        }else{
            log(" OK","green");
            log("\nBackup file : ","black");
            log(path,"blue");
            progressValue(100);
        }
    }
    if (action == WriteQCN)
    {

        log("\nLoading Data File...","black");
        int get1 = -1;
        int get2 = -1;
        if (!QLIB_NV_LoadNVsFromQCN(info.hndl,qpath.toLocal8Bit().data(),&get1,&get2))
        {
            log(" error","red");
        }else{
            log(" OK","green");
            log("\nWriting Data File to phone...","black");
            int res2;
            if (!QLIB_NV_WriteNVsToMobile(info.hndl,&res2))
            {
                log(" error","red");
            }else{
                log(" OK","green");
                progressValue(100);
            }

        }
    }
    QLIB_DisconnectServer(info.hndl);
}

MainThread::MainThread(QObject *parent)
    : QThread(parent)
{

}

MainThread *MainThread::instance_ = NULL;
MainThread *MainThread::Instance()
{
    return instance_;
}
void MainForm::progressText(QString log)
{
    ui->progressBar->setFormat(log);
}
void MainForm::log(QString log,QString color)
{
    ui->textEdit->setTextColor(color);
    ui->textEdit->insertPlainText(log);
    ui->textEdit->moveCursor(QTextCursor::End);
}
void MainForm::on_btn_read_qcn_clicked()
{
    ui->textEdit->clear();
    action = ReadQCN;
    cport = getSTR("(COM",")",ui->cb_comport->currentText()).toInt();
    thread->start(QThread::NormalPriority);
}

void MainForm::on_btn_write_qcn_clicked()
{
    ui->textEdit->clear();
    QString filename = QFileDialog::getOpenFileName(this,tr("Open XML File 1"),"", tr("QCN Files (*.qcn)"));
    if (filename == "")
    {
        log("Please select QCN file first!","red");
        return;
    }
    qpath = filename;
    action = WriteQCN;
    cport = getSTR("(COM",")",ui->cb_comport->currentText()).toInt();
    thread->start(QThread::NormalPriority);
}


void MainForm::on_btn_scan_port_clicked()
{
    ui->cb_comport->clear();
    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()){ ui->cb_comport->addItem(port.description()+" ("+port.portName()+")");}
}
