#include "mainform.h"

bool MainThread::ConnectDevice(DiagInfo &info)
{
    SetLibraryMode();
    if (!ConnectPort(info))
        return false;

    if (!SendSPC(info,"000000"))
        return false;

    if (!SetSIMDual(info,true))
        return false;

    EnableQcnNvItemCallBacks(info);
    return true;
}

void MainThread::SetLibraryMode()
{
    log("\nSetting QPST mode...","black");
    QLIB_SetLibraryMode(0);
    log(" OK","green");
}

bool MainThread::ConnectPort(DiagInfo & info)
{
    log("\nConnecting to phone...","black");
    info.hndl = QLIB_ConnectServer(info.portnum);
    unsigned int a = QLIB_IsPhoneConnected(info.hndl);
    if ( a )
    {
        log(" OK","green");
        unsigned long _iMSM_HW_Version = 0;
        unsigned long _iMobModel = 0;
        char _sMobSwRev[512];
        char _sModelStr[512];
        unsigned char bOk;
        bOk = QLIB_DIAG_EXT_BUILD_ID_F(info.hndl, &_iMSM_HW_Version,&_iMobModel, _sMobSwRev, _sModelStr );
        if (bOk)
        {
            log("\nMSM-HW ver : ","black");
            log(QString::number(_iMSM_HW_Version),"green");
            log("\nMobModel : ","black");
            log(QString::number(_iMobModel),"green");
            log("\nSoftware : ","black");
            log(_sMobSwRev,"green");
        }
        return true;
    }else{
        log(" error","red");
        log("\nPlease select currect diag port!","red");
        return false;
    }
}
bool MainThread::SetSIMDual(DiagInfo & info,bool dual)
{
    log("\nSet Multi Sim...","black");
    if (dual)
    {
        unsigned char res = QLIB_NV_SetTargetSupportMultiSIM(info.hndl,true);
        if (res)
        {
            log(" OK","green");
            return true;
        }else{
            log(" error","red");
            return false;
        }
    }else{
        if (QLIB_NV_SetTargetSupportMultiSIM(info.hndl,false))
        {
            log(" OK","green");
            return true;
        }else{
            log(" error","red");
            return false;
        }
    }
}


bool MainThread::SyncEFS(DiagInfo & info)
{
    log("\nSyncing EFS...","black");
    unsigned char b = 0;
    unsigned char b2 = 0;
    b = 47;
    b2 = 4;
    try
    {
        QLIB_EFS2_SyncWithWait(info.hndl, & b, 2000, & b2);
    }
    catch (...)
    {
        log(" error","red");
        log("\nCan not sync EFS","red");
        return false;
    }
    log(" OK","green");
    return true;
}
enum mode_enum_type {
    MODE_OFFLINE_A_F,
    MODE_OFFLINE_D_F,
    MODE_RESET_F,
    MODE_FTM_F,
            MODE_ONLINE_F,
            MODE_LPM_F,
            MODE_POWER_OFF_F,
            MODE_MAX_F};
bool MainThread::RebootNormal(DiagInfo & info)
{
    log("Rebooting phone...","black");
    QLIB_DIAG_CONTROL_F(info.hndl,MODE_OFFLINE_D_F);
    QThread::sleep(2000);
    QLIB_DIAG_CONTROL_F(info.hndl,MODE_RESET_F);
    log(" OK","green");
    return true;
}

void MainThread::qphoneNvToolHandlerCallBack(uint handle,ushort sid, ushort iNViD, ushort iNVToolFuncEnum, ushort iEvent, ushort iProgress)
{
    Q_UNUSED(handle)Q_UNUSED(sid)Q_UNUSED(iNViD)Q_UNUSED(iNVToolFuncEnum)Q_UNUSED(iEvent)
    instance_->progressText("Subscription_ID : "+QString::number(sid)+" | iNViD : "+QString::number(iNViD)+" | iNVToolFuncEnum : " +QString::number(iNVToolFuncEnum) + " | iEvent : " +QString::number(iEvent) +" | iProgress : %" +QString::number(iProgress));
    instance_->progressValue(iProgress);
}

void MainThread::EnableQcnNvItemCallBacks(DiagInfo & info)
{
    info.qphoneNvToolHandlerCallBack_delegate = nvToolCB(MainThread::qphoneNvToolHandlerCallBack);
    QLIB_NV_ConfigureCallBack(info.hndl, info.qphoneNvToolHandlerCallBack_delegate);
}


typedef struct{
    std::wstring imei;
    std::wstring tac;
    std::wstring fac;
    std::wstring snr;
    std::wstring svn;
    std::wstring luhnCode;
} Imei_Info;

QString MainThread::ReadIMEI(DiagInfo & info,int index)
{
    unsigned char array[128];
    std::wstring array2[15];
    int array3[15];
    unsigned short num1 = 4;
    unsigned char res = QLIB_DIAG_NV_READ_EXT_F(info.hndl, NV_UE_IMEI_I, array, index, 128, &num1);
    if (!res)
        return "000000000000000";
    int num = 0;
    for (int i = 1; i <= 8; i++)
    {
        if (i != 8)
        {
            array3[num] = static_cast<int>(array[i]);
            array3[num] &= 240;
            array3[num] >>= 4;
            array3[num + 1] = static_cast<int>(array[i + 1] & 15);
        }
        else
        {
            array3[num] = static_cast<int>(array[i]);
            array3[num] &= 240;
            array3[num] >>= 4;
        }
        num += 2;
    }
    Imei_Info imeiinfo;
    for (int j = 0; j < 15; j++)
    {
        array2[j] = std::to_wstring(array3[j]);
        if (j < 6)
        {
            imeiinfo.tac += array2[j];
        }
        else if (j >= 6 && j <= 7)
        {
            imeiinfo.fac += array2[j];
        }
        else if (j >= 7 && j <= 13)
        {
            imeiinfo.snr += array2[j];
        }
    }
    imeiinfo.luhnCode = array2[14];
    imeiinfo.imei = imeiinfo.tac + imeiinfo.fac + imeiinfo.snr + imeiinfo.luhnCode;
    if (imeiinfo.imei.size() != 15)
    {
        return "000000000000000";
    }
    return QString::fromWCharArray(imeiinfo.imei.c_str());
}



bool MainThread::SendSPC(DiagInfo & info,QString SPC)
{
    QString test = SPC;
    unsigned char test2[6];
    memcpy( test2, test.toStdString().c_str() ,test.size());
    log("\nSending SPC...","black");
    int piSPC_Result;

    if(QLIB_DIAG_SPC_F(info.hndl,test2,&piSPC_Result))
    {
        log(" OK","green");
        log("\nPhone IMEI1 : ","black");
        log(ReadIMEI(info,0),"green");
        log("\nPhone IMEI2 : ","black");
        log(ReadIMEI(info,1),"green");
        return true;
    }else{
        log(" error","red");
    }
    return true;
}
