#include "ScreenBuffer.h"

namespace Sherphy
{
    template <typename T>
    ScreenBuffer<T>::ScreenBuffer(size_t height, size_t width){
        m_buffer = SHERPHY_ALLOC(width * height);
        m_height = height;
        m_width = width;
    }

    template <typename T>
    ScreenBuffer<T>::~ScreenBuffer()
    {
        SHERPHY_DEALLOC(m_buffer);
    }

    template <typename T>
    void ScreenBuffer<T>::clear()
    {
        size_t buffer_size = m_width * m_height;
        for(size_t pixel_id = 0; pixel_id < buffer_size; pixel_id)
        {
            m_buffer[pixel_id].SetColor(0);
        }
        return;
    }

    template <typename T>
    void setBuffer(const T buffer, size_t width, size_t height)
    {
        memcpy(m_buffer, buffer, sizeof(T) * width * height);
        m_width = width;
        m_height = height;
    }


} // namespace Sherphy
