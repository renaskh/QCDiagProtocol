#ifndef QCLOADER_H
#define QCLOADER_H

#include "windows.h"

typedef void ( *nvToolCB )
(
   HANDLE hQMSLContext,
   unsigned short iSubscriptioniD,
   unsigned short iNVid,
   unsigned short iSourceFunc,
   unsigned short iEvent,
   unsigned short iProgress
);

typedef struct DiagInfo
{
    int portnum;
    HANDLE hndl;
    nvToolCB qphoneNvToolHandlerCallBack_delegate;
}DiagInfo;

#define NV_UE_IMEI_I 550

extern "C" {
    __declspec(dllimport) void QLIB_SetLibraryMode(unsigned char bUseQPST);
    __declspec(dllimport) HANDLE QLIB_ConnectServer(unsigned int iComPort);
    __declspec(dllimport) unsigned char QLIB_IsPhoneConnected(HANDLE hResourceContext);
    __declspec(dllimport) unsigned char QLIB_DIAG_EXT_BUILD_ID_F(HANDLE hResourceContext, unsigned long* piMSM_HW_Version, unsigned long* piMobModel,char* sMobSwRev, char* sModelStr);
    __declspec(dllimport) unsigned char QLIB_NV_SetTargetSupportMultiSIM( HANDLE hResourceContext, bool bTargetSupportMultiSIM);
    __declspec(dllimport) unsigned char QLIB_EFS2_SyncWithWait(HANDLE hResourceContext,unsigned char* sFilePath,unsigned long iTimeOut_ms,unsigned char* piExecutionStatus);
    __declspec(dllimport) unsigned char QLIB_DIAG_CONTROL_F(HANDLE hResourceContext, int eMode );
    __declspec(dllimport) void QLIB_NV_ConfigureCallBack( HANDLE hResourceContext, nvToolCB pNvToolCallback);
    __declspec(dllimport) unsigned char QLIB_DIAG_NV_READ_EXT_F(HANDLE hResourceContext, unsigned short iItemID, unsigned char* pItemData, unsigned short iContextID, int iLength, unsigned short* iStatus);
    __declspec(dllimport) unsigned char QLIB_DIAG_SPC_F(HANDLE hResourceContext, unsigned char iSPC[6], int* piSPC_Result);
    __declspec(dllimport) unsigned char QLIB_BackupNVFromMobileToQCN(HANDLE hResourceContext, char *sQCN_Path, int* iResultCode);
    __declspec(dllimport) unsigned char QLIB_NV_LoadNVsFromQCN(HANDLE hResourceContext, const char *sQCN_Path, int * iNumOfNVItemValuesLoaded, int *iResultCode);
    __declspec(dllimport) unsigned char QLIB_NV_WriteNVsToMobile(HANDLE hResourceContext, int *iResultCode);
    __declspec(dllimport) void QLIB_DisconnectServer( HANDLE hResourceContext );
}

#endif // QCLOADER_H
