#pragma once

#include "logging.h"

class Vector2D;
namespace SDK
{
	class CUserCmd;
}
namespace UTILS
{
	
	template<class T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi)
	{
		return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
	}
    template<unsigned int IIdx, typename TRet, typename ... TArgs>
    static auto CallVFunc(void* thisptr, TArgs ... argList) -> TRet
    {
        using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
    }
	template <class T>
	constexpr const T& Min(const T& x, const T& y)
	{
		return (x > y) ? y : x;
	}
	template<class T>
	static T* Find_Hud_Element(const char* name)
	{
		static auto pThis = *reinterpret_cast<DWORD**>(UTILS::FindSignature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 8D 58") + 1);
		static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(UTILS::FindSignature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39"));
		return (T*)find_hud_element(pThis, name);
	}

	template <class T>
	constexpr const T& Max(const T& x, const T& y)
	{
		return (x < y) ? y : x;
	}
	constexpr unsigned int FNVHashEx(const char* const data, const unsigned int value = 2166136261)
	{
		return (data[0] == '\0') ? value : (FNVHashEx(&data[1], (value * 16777619) ^ data[0]));
	}
	float GetLBYRotatedYaw(float lby, float yaw);
	bool IsPressingMovementKeys(SDK::CUserCmd* cmd);
	bool is_point_in_range(Vector2D, Vector2D, Vector2D);
	float GetFraction(float, float, float);
	float GetValueFromFraction(float, float, float);
	unsigned int GetNumberOfDigits(int);
	float RoundToDecimalPlaces(float, int);
	std::string FloatToString(float, int);
	Vector CalcAngle(Vector src, Vector dst);
	bool IsOnScreen(Vector origin, Vector& screen);
	float GetCurtime();
	float Lerp(float fraction, float min, float max);
	DWORD FindSignaturenew(const char* szModuleName, const char* PatternName, char* szPattern);
	uint64_t FindSignature(const char* szModule, const char* szSignature);
	DWORD WaitOnModuleHandle(std::string moduleName);
	bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask);
	DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);
	bool ClampLemon(Vector& angles);
}
