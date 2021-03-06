#include "Renderer.h"
#include "Materials/Material.h"
#include "Random.h"


std::unique_ptr<Image> Renderer::m_FinalImage;
std::vector<uint32_t> Renderer::m_ImageData;

std::shared_ptr<Scene> Renderer::m_Scene;
RendererProps Renderer::m_RendererProps;

ThreadPool Renderer::m_Pool;
RendererState Renderer::m_CurrentState;
Time Renderer::m_RenderTime;
Timer Renderer::m_Timer;


void Renderer::Init(const RendererProps& props)
{
	m_RendererProps = props;
	m_Pool.Init(m_RendererProps.ThreadsCount);
	m_CurrentState = RendererState::Nothing;
	m_RenderTime = Time(0);
}

void Renderer::Shutdown()
{
	//Fast join all working threads
	m_RendererProps.SamplesPerPixel = 0;

	m_Pool.Shutdown();
}

void Renderer::Render(const std::shared_ptr<Scene>& scene)
{
	m_CurrentState = RendererState::Rendering;
	m_Scene = scene;
	
	std::fill(begin(m_ImageData), end(m_ImageData), 0);

	m_Timer.Reset();

	std::vector<std::future<void>> futures;
	futures.reserve(m_RendererProps.ThreadsCount);

	for (uint32_t i = 0; i < m_RendererProps.ThreadsCount; ++i)
	{
		uint32_t startRow = i * m_FinalImage->GetHeight() / m_RendererProps.ThreadsCount;
		// Do not Render last row
		uint32_t endRow =
			glm::ceil(float(i + 1) * (float)(m_FinalImage->GetHeight() - 1) / (float)m_RendererProps.ThreadsCount);

		futures.push_back(m_Pool.AddTask(Renderer::RenderRange, m_Scene, startRow, endRow));
	}

	std::for_each(begin(futures), end(futures), [](const auto& future) { future.wait(); });

	m_RenderTime = m_Timer.ElapsedTime();
	m_CurrentState = RendererState::Nothing;
}

void Renderer::RenderRange(const std::shared_ptr<Scene>& scene, uint32_t startRow, uint32_t endRow)
{
	// Render from top to bottom 
	for (uint32_t y = endRow; y > startRow; --y)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x)
		{
			uint32_t index = x + y * m_FinalImage->GetWidth();
			glm::vec4 color(0);

			for (uint32_t i = 0; i < m_RendererProps.SamplesPerPixel; ++i)
			{
				glm::vec2 coord = {
					((float)x + Random::Float()) / (float)m_FinalImage->GetWidth(),
					((float)y + Random::Float()) / (float)m_FinalImage->GetHeight() };
				color += FragmentShader(coord);
			}

			color /= m_RendererProps.SamplesPerPixel;
			m_ImageData[index] = RGBAtoHEX(color);
			m_RenderTime = m_Timer.ElapsedTime();
		}
	}
}

void Renderer::UpdateImage()
{
	m_FinalImage->SetData(m_ImageData.data(), sizeof(std::vector<uint32_t>::value_type) * m_ImageData.size());
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (!m_FinalImage)
	{
		m_FinalImage = std::make_unique<Image>(width, height, ImageFormat::RGBA);
		m_FinalImage->Bind();
	}
	else
	{
		if (width == m_FinalImage->GetWidth() && height == m_FinalImage->GetHeight())
			return;
		m_FinalImage->Resize(width, height);
	}

	m_ImageData.resize(width * height);
}

#define RECURSIVE 1

#if not RECURSIVE
glm::vec4 Renderer::FragmentShader(glm::vec2 coord)
{
	Ray ray = m_Scene->Camera.CastRay(coord);
	HitRecord record;
	glm::vec3 color(1);

	constexpr glm::vec3 white(1.f);
	constexpr glm::vec3 background(0.f, 0.f, 0.f);

	for (uint32_t i = 0; i < m_RendererProps.ChildRaysCount; ++i)
	{
		if (m_Scene->Objects.Intersect(ray, Ray::MinLength, Ray::MaxLength, record))
		{
			glm::vec3 emitted = record.ObjectMaterial->Emitted(record.TexCoords, record.Point);
			glm::vec3 attenuation;

			if (!record.ObjectMaterial->Scatter(record, ray, attenuation))
			{
				return PostProcess(color * emitted);
				//color = emitted + background * color;
				//return PostProcess(color);
			}

			color = emitted + attenuation * color;
			continue;
		}
		
		glm::vec3 skyColor = background;//glm::mix(white, blue, 0.5f * (coord.y + 1));
		color *= skyColor;

		return PostProcess(color);
	}

	return glm::vec4(0, 0, 0, 1);
}

#else 

glm::vec4 Renderer::FragmentShader(glm::vec2 coord)
{
	Ray ray = m_Scene->Camera.CastRay(coord);
	
	glm::vec3 color = RayColor(ray, m_RendererProps.ChildRaysCount);

	return PostProcess(color);
}

#endif

glm::vec3 Renderer::RayColor(const Ray& ray, uint32_t depth)
{
	if (depth <= 0)
		return glm::vec3(0);

	HitRecord record;
	if (!m_Scene->Objects.Intersect(ray, Ray::MinLength, Ray::MaxLength, record))
		return m_Scene->Background;

	glm::vec3 emitted = record.ObjectMaterial->Emitted(record.TexCoords, record.Point);
	glm::vec3 attenuation;

	Ray scattered = ray;
	if (!record.ObjectMaterial->Scatter(record, scattered, attenuation))
		return emitted;

	return emitted + attenuation * RayColor(scattered, depth - 1);
}

glm::vec4 Renderer::PostProcess(const glm::vec3& color)
{
	glm::vec3 result;
	result = glm::clamp(color, 0.f, 1.f);
	result = glm::sqrt(result); // gamma correction
	return glm::vec4(result, 1);
}
