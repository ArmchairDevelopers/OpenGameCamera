#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <list>
#include <Psapi.h>
#include <string>

enum class PatternType
{
	Address,
	Pointer,
	PointerBYTE,
	PointerWORD,
	PointerDWORD,
	PointerQWORD,
	RelativePointer,
	RelativePointerBYTE,
	RelativePointerWORD,
	RelativePointerDWORD,
	RelativePointerQWORD,
};

typedef struct
{
	const char *pattern;
	PatternType type;
} Pattern;

#ifdef _WIN64
typedef DWORD64 XWORD;
#else
   typedef DWORD XWORD
#endif

class PatternScanner
{
private:
	template <typename T, typename ret> static ret ResolveRelativePtr(void *Address)
	{
		if (!Address)
			return NULL;

		T offset = *(T*)Address;
		if (!offset)
			return NULL;

		return (ret)((XWORD)Address + offset + sizeof(T));
	}

	template <typename T> static void *ResolveRelativePtr(void *Address)
	{
		return ResolveRelativePtr<T, void*>(Address);
	}

	template <typename T, typename ret> static ret ResolvePtr(void *Address)
	{
		if (!Address)
			return NULL;

		return (ret)*(T*)Address;
	}

	template <typename T> static void *ResolvePtr(void *Address)
	{
		return ResolvePtr<T, void*>(Address);
	}

public:
	static void *FindPattern(void *startAddress, DWORD searchSize, const Pattern &pattern);
	static void *FindPattern(const std::string &ModuleName, const Pattern &pattern);
	static void *FindPattern(const Pattern &pattern);

	static std::vector<void*> FindPatterns(const std::string &ModuleName, const Pattern &pattern);
	static std::vector<void*> FindPatterns(void *startAddress, DWORD searchSize, const Pattern &pattern);

	static void *FindPattern(void *startAddress, DWORD searchSize, Pattern *patterns, float *ratio = nullptr);
	static void *FindPattern(const std::string &ModuleName, Pattern *patterns, float *ratio = nullptr);
};