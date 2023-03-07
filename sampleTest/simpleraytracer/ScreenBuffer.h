#include <concepts>


namespace Sherphy{
    // template<typename T>
    // concept Pixel = requires(T px)
    // {
    //     px
    // }

    template<typename Pixel>
    class ScreenBuffer
    {
        public:
            ScreenBuffer(size_t width, size_t height);
            ~ScreenBuffer();
            void setbuffer(const Pixel, size_t width, size_t height);
            void clear();
        private:
            size_t m_width;
            size_t m_height;
            Pixel* m_buffer = nullptr;
    };
}