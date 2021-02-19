#ifndef __COMDAT_H__
#define __COMDAT_H__
BOOL sp_Break_Status;      //运行过程是否被中断
BOOL sp_bAllIgnore;        //是否忽略修改
BOOL sp_bManualFileName;   //自动给定输出文件名
BOOL sp_bWriteExist;       //文件存在是否直接覆盖
BOOL sp_bAddCopyNum;       //DLIS同名曲线增加拷贝数
BOOL sp_bAddFrameNo;       //DLIS同名曲线增加帧号
BOOL sp_bAddOrigin;        //DLIS同名曲线增加源
BOOL sp_bDirectory;        //是否建立目录
CStringArray sp_FilterCurves;    //只输入输出的曲线数组
#endif