

#if !defined(SherphyNoReturn)
	#if defined(_MSC_VER) && (_MSC_VER >=1300)
		#define SherphyNoReturn __declspec(noreturn)
	#else
		#define SherphyNoReturn [[noreturn]]
	#endif
#endif


