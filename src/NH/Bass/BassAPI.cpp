#include "BassAPI.h"

#include <Union/Dll.h>

namespace NH::Bass
{
	Pointers BASS{ nullptr };

	void LoadBassAPI()
	{
		Union::Dll* module = Union::Dll::Find("bass.dll");
		if (module == nullptr)
		{
			return;
		}

		BASS.BASS_GetDeviceInfo = (bool (__stdcall*)(::DWORD, BASS_DEVICEINFO*))module->GetProcedureAddress("BASS_GetDeviceInfo");
		BASS.BASS_Update = (bool (__stdcall*)(::DWORD))module->GetProcedureAddress("BASS_Update");
		BASS.BASS_SetConfig = (bool (__stdcall*)(::DWORD, ::DWORD))module->GetProcedureAddress("BASS_SetConfig");
		BASS.BASS_Init = (bool (__stdcall*)(::DWORD, ::DWORD, ::DWORD, HWND, const ::GUID*))module->GetProcedureAddress("BASS_Init");
		BASS.BASS_GetInfo = (bool (__stdcall*)(BASS_INFO*))module->GetProcedureAddress("BASS_GetInfo");
		BASS.BASS_StreamCreateFile = (HSTREAM (__stdcall*)(BOOL, const void*, ::DWORD, ::DWORD, ::DWORD))module->GetProcedureAddress("BASS_StreamCreateFile");
		BASS.BASS_ChannelStart = (bool (__stdcall*)(::DWORD))module->GetProcedureAddress("BASS_ChannelStart");
		BASS.BASS_ChannelFree = (bool (__stdcall*)(::DWORD))module->GetProcedureAddress("BASS_ChannelFree");
		BASS.BASS_ChannelSetAttribute = (bool (__stdcall*)(::DWORD, ::DWORD, float))module->GetProcedureAddress("BASS_ChannelSetAttribute");
		BASS.BASS_ChannelSlideAttribute = (bool (__stdcall*)(::DWORD, ::DWORD, float, ::DWORD))module->GetProcedureAddress("BASS_ChannelSlideAttribute");
		BASS.BASS_ChannelFlags = (bool (__stdcall*)(::DWORD, ::DWORD, ::DWORD))module->GetProcedureAddress("BASS_ChannelFlags");
		BASS.BASS_ChannelSetFX = (HFX (__stdcall*)(::DWORD, ::DWORD, ::DWORD))module->GetProcedureAddress("BASS_ChannelSetFX");
		BASS.BASS_FXSetParameters = (bool (__stdcall*)(HFX, const void*))module->GetProcedureAddress("BASS_FXSetParameters");
		BASS.BASS_ChannelGetLength = (QWORD (__stdcall*)(::DWORD, ::DWORD))module->GetProcedureAddress("BASS_ChannelGetLength");
		BASS.BASS_ChannelSeconds2Bytes = (QWORD (__stdcall*)(::DWORD, double))module->GetProcedureAddress("BASS_ChannelSeconds2Bytes");
		BASS.BASS_ChannelGetPosition = (QWORD (__stdcall*)(::DWORD, ::DWORD))module->GetProcedureAddress("BASS_ChannelGetPosition");
		BASS.BASS_ChannelBytes2Seconds = (QWORD (__stdcall*)(::DWORD, QWORD))module->GetProcedureAddress("BASS_ChannelBytes2Seconds");
		BASS.BASS_ChannelSetSync = (HSYNC (__stdcall*)(::DWORD, ::DWORD, QWORD, SYNCPROC, void*))module->GetProcedureAddress("BASS_ChannelSetSync");
		BASS.BASS_ChannelGetAttribute = (bool (__stdcall*)(::DWORD, ::DWORD, float*))module->GetProcedureAddress("BASS_ChannelGetAttribute");
		BASS.BASS_ErrorGetCode = (int (__stdcall*)())module->GetProcedureAddress("BASS_ErrorGetCode");
	}
}