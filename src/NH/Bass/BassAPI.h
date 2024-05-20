#pragma once

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include <bass.h>

namespace NH::Bass
{
	extern struct Pointers
	{
		bool (__stdcall *BASS_GetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info) = nullptr;
		bool (__stdcall *BASS_Update)(DWORD length) = nullptr;
		bool (__stdcall *BASS_SetConfig)(DWORD option, ::DWORD value) = nullptr;
		bool (__stdcall *BASS_Init)(::DWORD device, ::DWORD freq, ::DWORD flags, HWND win, const ::GUID *clsid) = nullptr;
		bool (__stdcall *BASS_GetInfo)(BASS_INFO *info) = nullptr;
		HSTREAM (__stdcall *BASS_StreamCreateFile)(BOOL mem, const void *file, ::DWORD offset, ::DWORD length, ::DWORD flags) = nullptr;
		bool (__stdcall *BASS_ChannelStart)(::DWORD handle) = nullptr;
		bool (__stdcall *BASS_ChannelFree)(::DWORD handle) = nullptr;
		bool (__stdcall *BASS_ChannelSetAttribute)(::DWORD handle, ::DWORD attrib, float value) = nullptr;
		bool (__stdcall *BASS_ChannelSlideAttribute)(::DWORD handle, ::DWORD attrib, float value, ::DWORD time) = nullptr;
		bool (__stdcall *BASS_ChannelFlags)(::DWORD handle, ::DWORD flags, ::DWORD mask) = nullptr;
		HFX (__stdcall *BASS_ChannelSetFX)(::DWORD handle, ::DWORD type, ::DWORD priority) = nullptr;
		bool (__stdcall *BASS_FXSetParameters)(HFX handle, const void *params) = nullptr;
		QWORD (__stdcall *BASS_ChannelGetLength)(::DWORD handle, ::DWORD mode) = nullptr;
		QWORD (__stdcall *BASS_ChannelSeconds2Bytes)(::DWORD handle, double pos) = nullptr;
		QWORD (__stdcall *BASS_ChannelGetPosition)(::DWORD handle, ::DWORD mode) = nullptr;
		QWORD (__stdcall *BASS_ChannelBytes2Seconds)(::DWORD handle, QWORD pos) = nullptr;
		HSYNC (__stdcall *BASS_ChannelSetSync)(::DWORD handle, ::DWORD type, QWORD param, SYNCPROC sync, void *user) = nullptr;
		bool (__stdcall *BASS_ChannelGetAttribute)(::DWORD handle, ::DWORD attrib, float *value) = nullptr;
		int (__stdcall *BASS_ErrorGetCode)() = nullptr;
	} BASS;

	void LoadBassAPI();
}