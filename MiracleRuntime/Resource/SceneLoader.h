#pragma once

namespace Sherphy 
{
	class NormalScene;
	class SceneLoader 
	{
	public:
		static void LoadScene(NormalScene* data_base);
		static void LoadATestScene(NormalScene* data_base);
	};
}