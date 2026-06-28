#pragma once

#ifdef _WINDOWS
#ifdef ENGINE_EXPORTS
#define ENG_API __declspec(dllexport)
#else
#define ENG_API __declspec(dllimport)
#endif

#pragma warning(disable : 4251)
#else
#define ENG_API
#endif