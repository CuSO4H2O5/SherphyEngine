

#if !defined(SherphyNoReturn)
	#if defined(_MSC_VER) && (_MSC_VER >=1300)
		#define SherphyNoReturn __declspec(noreturn)
	#else
		#define SherphyNoReturn [[noreturn]]
	#endif
#endif

#if !defined(SHERPHY_API)
	#if defined(SHERPHY_DLL)
		#if defined(SHERPHY_IMPORT_API)
			#if defined(_MSC_VER)
			#define SHERPHY_API      __declspec(dllimport)
			#else
			#define SHERPHY_API      __attribute(dllimport)
			#endif
		#else defined(SHERPHY_SHARED_API)
			#if defined(_MSC_VER)
			#define SHERPHY_API      __declspec(dllexport)
			#else
			#define SHERPHY_API      __attribute(dllexport)
			#endif
		#endif
	#else
		#define SHERPHY_API
	#endif
#endif
