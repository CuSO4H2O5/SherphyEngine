#include "Scene.h"


namespace Sherphy 
{
	void NormalScene::clear() 
	{
		m_cameras.clear();
		m_data_base.clear();
		m_scene_objects.clear();
		m_main_camera_id = NO_MAIN_CAMERA;
	}
}