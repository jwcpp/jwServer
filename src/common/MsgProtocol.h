#pragma once


enum MsgProtocol
{
	eMSGID_SessionVerify = 100,
	eMSGID_SessionVerifyRet,


	//c2s
	eMSGID_C2S_Login,
	eMSGID_S2C_LoginFail,

	//s2s
	eMSGID_W2D_Login,
	eMSGID_S2D_LoadRole,
	eMSGID_D2S_LoadRole,
	eMSGID_S2D_CreatePlayer,
	eMSGID_S2D_SavePlayer,
	eMSGID_S2D_Offline,
};