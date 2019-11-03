#include "SigScan.h"

DWORD GetModuleSize(const char* module)
{
	MODULEINFO hModInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(module), &hModInfo, sizeof(hModInfo));
	return hModInfo.SizeOfImage;
}

void *PatternScanner::FindPattern(const std::string &ModuleName, const Pattern &pattern)
{
	return PatternScanner::FindPattern(GetModuleHandle(ModuleName.c_str()), GetModuleSize(ModuleName.c_str()), pattern);
}

void *PatternScanner::FindPattern(const Pattern &pattern)
{
	MODULEINFO hModInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &hModInfo, sizeof(hModInfo));
	return PatternScanner::FindPattern(GetModuleHandle(NULL), hModInfo.SizeOfImage, pattern);
}

std::vector<void*> PatternScanner::FindPatterns(const std::string &ModuleName, const Pattern &pattern)
{
	return PatternScanner::FindPatterns(GetModuleHandle(ModuleName.c_str()), GetModuleSize(ModuleName.c_str()), pattern);
}

std::vector<void*> PatternScanner::FindPatterns(void *startAddress, DWORD searchSize, const Pattern &pattern)
{
	std::vector<void*> ret;

	void *base = startAddress;
	DWORD size = searchSize;
	void *addr = base;
	do
	{
		addr = PatternScanner::FindPattern((void*)((XWORD)addr + 1), size - ((XWORD)addr - (XWORD)base + 1), pattern);
		if (addr)
			ret.push_back(addr);
	} while (addr != nullptr);

	return ret;
}

void *PatternScanner::FindPattern(void *startAddress, DWORD searchSize, const Pattern &pattern)
{
	size_t len = strlen(pattern.pattern);
	if (len == 0)
		return nullptr;

	XWORD pos = 0;

	int byteCount = 1;
	uint32_t i = 0;
	while (i < len - 1)
	{
		if (pattern.pattern[i] == ' ')
			byteCount++;
		i++;
	}

	byte *patt = (byte*)malloc(byteCount + 1);
	if (!patt)
		return nullptr;

	byte *mask = (byte*)malloc(byteCount + 1);
	if (!mask)
		return nullptr;

	int offset = 0;
	int bytesCounted = 0;
	i = 0;
	while (i < len - 1)
	{
		if (pattern.pattern[i] == '[')
		{
			i++;
			offset = bytesCounted;
		}

		if (pattern.pattern[i] == '\0')
			break;

		if (pattern.pattern[i] == '?' && pattern.pattern[i + 1] == '?')
		{
			mask[bytesCounted] = '?';
			patt[bytesCounted] = '\0';
		}
		else
		{
			byte hn = pattern.pattern[i] > '9' ? pattern.pattern[i] - 'A' + 10 : pattern.pattern[i] - '0';
			byte ln = pattern.pattern[i + 1] > '9' ? pattern.pattern[i + 1] - 'A' + 10 : pattern.pattern[i + 1] - '0';
			byte n = (hn << 4) | ln;

			mask[bytesCounted] = 'x';
			patt[bytesCounted] = n;
		}

		bytesCounted++;

		i += 2;
		while (i < len && (pattern.pattern[i] == ' ' || pattern.pattern[i] == '\t' || pattern.pattern[i] == '\r' || pattern.pattern[i] == '\n'))
			i++;
	}
	mask[bytesCounted] = '\0';

	size_t searchLen = strlen((char*)&mask[0]) - 1;

	void *ret = nullptr;
	for (XWORD retAddress = (XWORD)startAddress; retAddress < (XWORD)startAddress + searchSize; retAddress++)
	{
		__try
		{
			if (*(byte*)retAddress == patt[pos] || mask[pos] == '?')
			{
				if (mask[pos + 1] == '\0')
				{
					ret = (void*)(retAddress - searchLen + offset);
					break;
				}

				pos++;
			}
			else
				pos = 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			pos = 0;
		}
	}

	free(patt);
	free(mask);

	switch (pattern.type)
	{
	case PatternType::Pointer:
		ret = PatternScanner::ResolvePtr<XWORD>(ret);
		break;

	case PatternType::PointerBYTE:
		ret = PatternScanner::ResolvePtr<BYTE>(ret);
		break;

	case PatternType::PointerWORD:
		ret = PatternScanner::ResolvePtr<WORD>(ret);
		break;

	case PatternType::PointerDWORD:
		ret = PatternScanner::ResolvePtr<DWORD>(ret);
		break;

	case PatternType::PointerQWORD:
		ret = PatternScanner::ResolvePtr<DWORD64>(ret);
		break;

	case PatternType::RelativePointer:
		ret = PatternScanner::ResolveRelativePtr<int32_t>(ret);
		break;

	case PatternType::RelativePointerBYTE:
		ret = PatternScanner::ResolveRelativePtr<BYTE>(ret);
		break;

	case PatternType::RelativePointerWORD:
		ret = PatternScanner::ResolveRelativePtr<WORD>(ret);
		break;

	case PatternType::RelativePointerDWORD:
		ret = PatternScanner::ResolveRelativePtr<DWORD>(ret);
		break;

	case PatternType::RelativePointerQWORD:
		ret = PatternScanner::ResolveRelativePtr<DWORD64>(ret);
		break;
	}

	return ret;
}

void *PatternScanner::FindPattern(void *startAddress, DWORD searchSize, Pattern *patterns, float *ratio)
{
	int totalCount = 0;
	typedef struct
	{
		void *addr;
		int count;
	} result;
	std::list<result> results;
#ifdef DEVELOPER
	std::list<void*> rawResults;
#endif

	int bestCount = 0;
	void *ret = nullptr;

	while (true)
	{
		Pattern pattern = patterns[totalCount];
		if (!pattern.pattern)
			break;

		totalCount++;

		void *addr = PatternScanner::FindPattern(startAddress, searchSize, pattern);
#ifdef DEVELOPER
		rawResults.push_back(addr);
#endif
		if (!addr)
			continue;

		bool found = false;
		for (result &res : results)
		{
			if (res.addr == addr)
			{
				res.count++;
				if (res.count > bestCount)
				{
					ret = addr;
					bestCount = res.count;
				}
				found = true;
				break;
			}
		}

		if (!found)
		{
			result res =
			{
				addr,
				1
			};
			results.push_back(res);
			if (bestCount == 0)
			{
				bestCount = 1;
				ret = addr;
			}
		}
	}

#ifdef DEVELOPER
	bool success = false;
	if (bestCount == 0)
		log("Patterns are broken. Hack won't work.");
	else if (bestCount == 1 && totalCount > 1)
		log("Unable to determine majority of scan results, the hack most likely won't work.");
	else if (bestCount < totalCount)
		log("Some patterns seem to be broken (Found %d/%d).", bestCount, totalCount);
	else
		success = true;

	if (!success)
	{
		int i = 0;
		for (auto &addr : rawResults)
		{
			if (!addr || addr != ret)
				log("Broken pattern: %d (%p)", i, addr);
			i++;
		}
	}
#endif

	if (ratio)
		*ratio = (float)bestCount / totalCount;

	return ret;
}

void *PatternScanner::FindPattern(const std::string &ModuleName, Pattern *patterns, float *ratio)
{
	return PatternScanner::FindPattern(GetModuleHandle(ModuleName.c_str()), GetModuleSize(ModuleName.c_str()), patterns, ratio);
}