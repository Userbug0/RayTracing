#include <cstdint>

typedef unsigned int GLenum;


enum class ImageFormat
{
	None = 0,
	RGB = 1,
	RGBA = 2
};


class Image
{
public:
	Image(uint32_t width, uint32_t height, ImageFormat format);
	~Image();

	void Bind(uint32_t slot = 0) const;
	void UnBind() const;

	inline uint32_t GetWidth() { return m_Width; }
	inline uint32_t GetHeight() { return m_Height; }
	inline uint32_t GetTextureID() { return m_TextureID; }

	void SetData(const void* data, uint32_t size);

private:
	uint32_t m_TextureID;

	GLenum m_DataFormat;
	GLenum m_InternalFormat;

	uint32_t m_Width;
	uint32_t m_Height;
};
