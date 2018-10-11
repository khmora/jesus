#pragma once

#include "../UTILS/offsets.h"
#include "../UTILS/interfaces.h"
#include "CBaseEntity.h"


namespace SDK
{
	class CSWeaponInfo
	{
	public:
		char _0x0000[20];
		__int32 max_clip;			//0x0014 
		char _0x0018[12];
		__int32 max_reserved_ammo;	//0x0024 
		char _0x0028[96];
		char* hud_name;				//0x0088 
		char* weapon_name;			//0x008C 
		char _0x0090[60];
		__int32 WeaponType;				//0x00CC 
		__int32 price;				//0x00D0 
		__int32 reward;				//0x00D4 
		char _0x00D8[20];
		BYTE full_auto;				//0x00EC 
		char _0x00ED[3];
		__int32 damage;				//0x00F0 
		float armor_ratio;			//0x00F4 
		__int32 bullets;			//0x00F8 
		float penetration;			//0x00FC 
		char _0x0100[8];
		float range;				//0x0108 
		float range_modifier;		//0x010C 
		char _0x0110[16];
		BYTE silencer;				//0x0120 
		char _0x0121[15];
		float max_speed;			//0x0130 
		float max_speed_alt;		//0x0134 
		char _0x0138[76];
		__int32 recoil_seed;		//0x0184 
		char _0x0188[32];
	};

	struct Weapon_Info
	{
	public:
		char _0x0000[20];
		__int32 iMaxClip1;            //0x0014 
		char _0x0018[12];
		__int32 max_reserved_ammo;    //0x0024 
		char _0x0028[96];
		char* hud_name;                //0x0088 
		char* weapon_name;            //0x008C 
		char _0x0090[60];
		__int32 WeaponType;                //0x00CC 
		__int32 price;                //0x00D0 
		__int32 reward;                //0x00D4 
		char _0x00D8[20];
		BYTE full_auto;                //0x00EC 
		char _0x00ED[3];
		__int32 iDamage;                //0x00F0 
		float flArmorRatio;            //0x00F4 
		__int32 bullets;            //0x00F8 
		float flPenetration;            //0x00FC 
		char _0x0100[8];
		float flRange;                //0x0108 
		float flRangeModifier;        //0x010C 
		char _0x0110[16];
		BYTE silencer;                //0x0120 
		char _0x0121[15];
		float max_speed;            //0x0130 
		float max_speed_alt;        //0x0134 
		char _0x0138[76];
		__int32 flInaccuracyMove;        //0x0184 
		char _0x0188[32];
	};

	/*enum class ItemDefinitionIndex : int
	{
	INVALID = -1,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	MAX_ITEMDEFINITIONINDEX
	};*/

	

	class CBaseWeapon
	{
	public:
		char* GetGunIcon()
		{
			short WeaponId = this->GetItemDefenitionIndex();
			switch (WeaponId)
			{
			case WEAPON_DEAGLE:
			case WEAPON_REVOLVER:
				return "F";
			case WEAPON_ELITE:
				return "S";
			case WEAPON_FIVESEVEN:
				return "U";
			case WEAPON_GLOCK:
			case WEAPON_TEC9:
				return "C";
			case WEAPON_AK47:
				return "B";
			case WEAPON_AUG:
				return "E";
			case WEAPON_AWP:
				return "R";
			case WEAPON_FAMAS:
				return "T";
			case WEAPON_G3SG1:
			case WEAPON_SCAR20:
				return "I";
			case WEAPON_GALILAR:
				return "V";
			case WEAPON_M249:
				return "Z";
			case WEAPON_M4A1:
			case WEAPON_M4A1_SILENCER:
				return "W";
			case WEAPON_MAC10:
				return "L";
			case WEAPON_P90:
				return "M";
			case WEAPON_UMP45:
				return "Q";
			case WEAPON_XM1014:
				return "]";
			case WEAPON_BIZON:
			case WEAPON_MP9:
				return "D";
			case WEAPON_MAG7:
			case WEAPON_SAWEDOFF:
			case WEAPON_NOVA:
				return "K";
			case WEAPON_NEGEV:
				return "Z";
			case WEAPON_USP_SILENCER:
			case WEAPON_TASER:
			case WEAPON_HKP2000:
			case WEAPON_P250:
			case WEAPON_CZ75A:
				return "Y";
			case WEAPON_MP7:
				return "X";
			case WEAPON_SG556:
				return "[";
			case WEAPON_SSG08:
				return "N";
			case WEAPON_FLASHBANG:
			case WEAPON_DECOY:
				return "G";
			case WEAPON_HEGRENADE:
			case WEAPON_MOLOTOV:
			case WEAPON_INCGRENADE:
				return  "H";
			case WEAPON_SMOKEGRENADE:
				return "P";
			case WEAPON_C4:
				return "\\";
			case WEAPON_KNIFE_CT:
			case WEAPON_KNIFE_T:
			case WEAPON_KNIFE_BAYONET:
			case WEAPON_KNIFE_FLIP:
			case WEAPON_KNIFE_GUT:
			case WEAPON_KNIFE_KARAMBIT:
			case WEAPON_KNIFE_M9_BAYONET:

			case WEAPON_KNIFE_FALCHION:
			case WEAPON_KNIFE_BOWIE:
			case WEAPON_KNIFE_BUTTERFLY:

				return "J";
			}
		}
		float GetNextPrimaryAttack()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flNextPrimaryAttack);
		}
		short GetItemDefenitionIndex()
		{
			return *reinterpret_cast<short*>(uintptr_t(this) + OFFSETS::m_iItemDefinitionIndex);
		}

		inline int* GetFallbackPaintKit() {
			// DT_BaseAttributableItem -> m_nFallbackPaintKit
			return (int*)((DWORD)this + 0x3170);
		}
		short fix()
		{
			return *(short*)((uintptr_t)this + OFFSETS::m_iItemDefinitionIndex);
		}

		float GetPostponeFireReadyTime()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flPostponeFireReadyTime);
		}
		float GetSpreadCone()
		{
			typedef float(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 437)(this);
		}
		float GetInaccuracy()
		{
			typedef float(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 467)(this); //469
		}
		HANDLE GetOwnerHandle()
		{
			return *(HANDLE*)((uintptr_t)this + 0x000031D0);
		}
		Vector GetOrigin()
		{
			return *(Vector*)((uintptr_t)this + 0x00000134);
		}
		bool is_revolver()
		{
			short iWeaponID = this->GetItemDefenitionIndex();
			return (iWeaponID == 64);
		}

		bool is_knife()
		{
			short iWeaponID = this->GetItemDefenitionIndex();
			return (iWeaponID == 42 || iWeaponID == 59
				|| iWeaponID == 59 || iWeaponID == 41
				|| iWeaponID == 500 || iWeaponID == 505 || iWeaponID == 506
				|| iWeaponID == 507 || iWeaponID == 508 || iWeaponID == 509
				|| iWeaponID == 515);
		}

		bool is_grenade()
		{
			if (!this)
				return false;

			short WeaponId = this->GetItemDefenitionIndex();

			if (!WeaponId)
				return false;

			if (WeaponId == 43 || WeaponId == 44 || WeaponId == 45 || WeaponId == 46 || WeaponId == 47 || WeaponId == 48)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		int GetLoadedAmmo()
		{
			return *(int*)((DWORD)this + 0x3234);
		}
		void UpdateAccuracyPenalty()
		{
			typedef void(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 468)(this); //470
		}
		Weapon_Info* GetWeaponInfo()
		{
			if (!this) return nullptr;
			typedef Weapon_Info*(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 444)(this);
		}
		CSWeaponInfo* get_full_info()
		{
			if (!this) return nullptr;
			typedef CSWeaponInfo*(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 444)(this);

		}
	};

	class CCSBomb
	{
	public:
		HANDLE GetOwnerHandle()
		{
			return *(HANDLE*)((uintptr_t)this + 0x000031D0);
		}

		float GetC4BlowTime()
		{
			return *(float*)((uintptr_t)this + 0x0000297C);
		}

		float GetC4DefuseCountDown()
		{
			return *(float*)((uintptr_t)this + 0x00002994);
		}

		int GetBombDefuser()
		{
			return *(int*)((uintptr_t)this + 0x000017B8);
		}

		bool IsBombDefused()
		{
			return *(bool*)((uintptr_t)this + 0x00002998);
		}
	};

	class CBaseCSGrenade : CBaseWeapon
	{
	public:
		float pin_pulled()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_bPinPulled);
		}

		float throw_time()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_fThrowTime);
		}
	};

	class CCSGrenade
	{
	public:

		float GetThrowTime()
		{
			return *(float*)((uintptr_t)this + OFFSETS::m_fThrowTime);
		}
	};
}