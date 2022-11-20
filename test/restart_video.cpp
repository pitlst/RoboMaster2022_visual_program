#include "MvCameraControl.h"
#include "malloc.h"
#include <cstring>

int main()
{
    void *handle = nullptr;
    int nRet;
    //帧数据指针
    unsigned char *pData;
    //帧数据大小
    unsigned int nDataSize;
    //输出帧的信息
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    //设备信息
    MV_CC_DEVICE_INFO *pDeviceInfo;
    do
    {
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
        //枚举设备
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet)
        {
            break;
        }
        if (stDeviceList.nDeviceNum > 0)
        {
            for (int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                auto pDeviceInfo = stDeviceList.pDeviceInfo[i];
                if (nullptr == pDeviceInfo)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
        unsigned int nIndex = 0;
        if (nIndex >= stDeviceList.nDeviceNum)
        {
            break;
        }
        //选择设备并创建句柄
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
        if (MV_OK != nRet)
        {
            break;
        }
        //打开设备
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            break;
        }
        return 0;
        //获取数据包大小
        MVCC_INTVALUE stParam;
        memset(&stParam, 0, sizeof(MVCC_INTVALUE));
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
        if (MV_OK != nRet)
        {
            break;
        }

        memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
        pData = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
        if (nullptr == pData)
        {
            break;
        }
        nDataSize = stParam.nCurValue;

        auto nRet = MV_CC_GetOneFrameTimeout(handle, pData, nDataSize, &stImageInfo, 1000);
        if (nRet != MV_OK)
        {
            break;
        }
    } while (0);
    //如果不正常初始化需要释放资源
    if (nRet != MV_OK)
    {
        if (handle != nullptr)
        {
            MV_CC_DestroyHandle(handle);
            handle = nullptr;
        }
    }
    return 0;
}