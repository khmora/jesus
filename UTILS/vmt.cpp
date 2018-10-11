#include "vmt.h"

namespace VMT
{
	VMTHookManager::VMTHookManager(void* instance)
	{
		Init(instance);
	}
	VMTHookManager::~VMTHookManager()
	{
		Restore();
	}

	void VMTHookManager::Init(void* instance)
	{
		m_instance = instance;
		m_vtable = *static_cast<void***>(instance);

		/// gets the ammount of vfuncs in the vtable
		auto GetSize = [this]() -> int
		{
			int count = 0;
			for (void* curr_func = m_vtable[count]; curr_func != nullptr;)
			{
				MEMORY_BASIC_INFORMATION mbi;
				if (!VirtualQuery(curr_func, &mbi, sizeof(mbi)))
					break;

				/// offlimits
				if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))
					break;

				/// not readable
				if (!(mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_WRITECOPY)))
					break;

				count++;
				curr_func = m_vtable[count];
			}

			return count;
		}; m_size = GetSize();
	}
	void VMTHookManager::Restore()
	{
		if (!m_instance || !m_vtable)
			return;

		/// restore all the original functions to the vtable
		for (const auto &func : m_original_funcs)
		{
			unsigned long old_protection, tmp;
			VProtect(&m_vtable[func.first], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protection);
			m_vtable[func.first] = func.second;
			VProtect(&m_vtable[func.first], sizeof(void*), old_protection, &tmp);
		}

		m_original_funcs.clear();
	}
	void VMTHookManager::UnHookFunction(int index)
	{
		/// index is out of range
		if (index < 0 || index >= m_size)
			return;

		/// not even hooked retard
		if (m_original_funcs.find(index) == m_original_funcs.end())
			return;

		auto original = m_original_funcs[index];
		m_original_funcs.erase(index);

		unsigned long old_protection, tmp;
		VProtect(&m_vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protection);
		m_vtable[index] = original;
		VProtect(&m_vtable[index], sizeof(void*), old_protection, &tmp);
	}
	void VMTHookManager::VProtect(void* address, unsigned int size, unsigned long new_protect, unsigned long* old_protect)
	{
		VirtualProtect(address, size, new_protect, old_protect);
	}
	/*CVMTHookManager::CVMTHookManager() {}
	CVMTHookManager::~CVMTHookManager()
	{
		this->HookTable(false);
	}
	CVMTHookManager::CVMTHookManager(void* Interface)
	{
		this->Init(Interface);
	}
	bool CVMTHookManager::Init(void* Interface)
	{
		pOrgTable = *(void**)Interface;
		this->count = this->GetCount();
		pCopyTable = malloc(sizeof(void*)* count);
		memcpy(pCopyTable, pOrgTable, sizeof(void*) * count);
		pObject = (DWORD*)Interface;
		return true;
		/*m_ClassBase = Interface;
		m_OldVT = *Interface;
		m_VTSize = GetVTCount(*Interface);
		m_NewVT = new DWORD[m_VTSize + 1];
		memcpy(&m_NewVT[1], m_OldVT, sizeof(DWORD) * m_VTSize);
		m_NewVT[0] = (uintptr_t)m_OldVT[-1];
		DWORD old;
		VirtualProtect(Interface, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*Interface = &m_NewVT[1];
		VirtualProtect(Interface, sizeof(DWORD), old, &old);
		return true;*/
	/*}
	void CVMTHookManager::HookTable(bool hooked)
	{
		if (hooked)
			*pObject = (DWORD)pCopyTable;
		else
			*pObject = (DWORD)pOrgTable;
	}
	void* CVMTHookManager::HookFunction(int Index, void* hkFunction)
	{
		if (Index < this->count && Index >= 0)
		{
			((DWORD*)pCopyTable)[Index] = (DWORD)hkFunction;
			return (void*)((DWORD*)pOrgTable)[Index];
		}
		return NULL;
	}
	bool CVMTHookManager::CanReadPointer(void* table)
	{
		MEMORY_BASIC_INFORMATION mbi;

		if (table == nullptr)
			return false;
		if (!VirtualQuery(table, &mbi, sizeof(mbi))) 
			return false;
		if (mbi.Protect & k_page_offlimits) 
			return false;

		return mbi.Protect & k_page_readable;
	}
	int CVMTHookManager::GetCount()
	{
		int index = 0; //0
		void** table = ((void**)pOrgTable);
		for (void* fn; (fn = table[index]) != nullptr; index++)
		{
			if (!this->CanReadPointer(fn)) 
				break;
		}
		return index;
	}
	DWORD CVMTHookManager::GetVTCount(PDWORD pdwVMT)
	{
		DWORD dwIndex = 0;
		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
		{
			if (IS_INTRESOURCE((FARPROC)pdwVMT[dwIndex]))
				break;
			}
		return dwIndex;
	}*/
}
bool IsBadReadPtrxd(PVOID pPointer)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	if (VirtualQuery(pPointer, &mbi, sizeof(mbi)))
	{
		DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);

		bool ret = !(mbi.Protect & mask);

		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))
			ret = true;

		return ret;
	}

	return true;
}
namespace CSX
{
	namespace Hook
	{
		PVOID WriteVTable(PVOID pTablePtr, PVOID pFuncAddress, DWORD dwIndex)
		{
			if (IsBadReadPtr(pTablePtr, sizeof(PVOID)))
				return nullptr;

			DWORD dwOffset = dwIndex * sizeof(PVOID);

			PVOID pFunc = (PVOID)((DWORD)pTablePtr + dwOffset);
			PVOID Func_o = (PVOID)*(PDWORD)pFunc;

			if (IsBadReadPtr(pFunc, sizeof(PVOID)))
				return nullptr;

			DWORD dwOld = 0;
			VirtualProtect(pFunc, sizeof(PVOID), PAGE_READWRITE, &dwOld);

			*(PDWORD)pFunc = (DWORD)pFuncAddress;

			VirtualProtect(pFunc, sizeof(PVOID), dwOld, &dwOld);

			return Func_o;
		}

		VTable::VTable()
		{
			pPtrPtrTable = nullptr;
			pPtrOldTable = nullptr;
			pPtrNewTable = nullptr;
			pPtrPtrTable = nullptr;

			dwCountFunc = 0;
			dwSizeTable = 0;
		}

		bool VTable::InitTable(PVOID pTablePtrPtr)
		{
			if (IsBadReadPtr(pTablePtrPtr, sizeof(PVOID)))
				return false;

			pPtrPtrTable = (PVOID*)pTablePtrPtr;
			pPtrOldTable = (PVOID*)(*(PDWORD)pPtrPtrTable);

			while (!IsBadReadPtrxd(pPtrOldTable[dwCountFunc]))
				dwCountFunc++;

			//while ( !IsBadCodePtr( (FARPROC)pPtrOldTable[dwCountFunc] ) && !CSX::Utils::IsBadReadPtr( pPtrOldTable[dwCountFunc] ) )
			//	dwCountFunc++;

			if (dwCountFunc)
			{
				dwSizeTable = sizeof(PVOID) * dwCountFunc;

				pPtrNewTable = (PVOID*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSizeTable);
				memcpy(pPtrNewTable, pPtrOldTable, dwSizeTable);

				*(PDWORD)pPtrPtrTable = (DWORD)pPtrNewTable;

				return true;
			}

			return false;
		}

		void VTable::HookIndex(DWORD dwIndex, PVOID pNewAddress)
		{
			if (pPtrNewTable)
				((PVOID*)pPtrNewTable)[dwIndex] = pNewAddress;
		}

		PVOID VTable::GetFuncAddress(DWORD dwIndex)
		{
			if (pPtrOldTable)
			{
				PVOID pAddres = ((PVOID*)pPtrOldTable)[dwIndex];
				return pAddres;
			}

			return nullptr;
		}

		PVOID VTable::GetHookIndex(DWORD dwIndex, PVOID pNewAddress)
		{
			if (pPtrNewTable)
			{
				PVOID pOld = ((PVOID*)pPtrNewTable)[dwIndex];
				((PVOID*)pPtrNewTable)[dwIndex] = pNewAddress;
				return pOld;
			}

			return nullptr;
		}

		void VTable::UnHook()
		{
			if (!IsBadReadPtrxd(pPtrPtrTable))
				*(PDWORD)pPtrPtrTable = (DWORD)pPtrOldTable;
		}

		void VTable::ReHook()
		{
			if (!IsBadReadPtrxd(pPtrPtrTable))
				*(PDWORD)pPtrPtrTable = (DWORD)pPtrNewTable;
		}
	}
}