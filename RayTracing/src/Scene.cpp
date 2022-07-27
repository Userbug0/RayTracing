#include "Scene.h"

#include "Objects/Sphere.h"
#include "Objects/BVH.h"
#include "Objects/Rect.h"
#include "Materials/Material.h"
#include "Random.h"


void Scene::LoadSandBoxScene()
{
    CameraOrientation orientation;
    orientation.Position = { 0, 0, 1 };
    orientation.LookAt = { 0, 0, -3 };
    orientation.Up = { 0, 1, 0 };

    CameraProps properties;
    properties.AspectRatio = 16.f / 9.f;
    properties.FOV = glm::radians(45.f);
    properties.FocusDist = glm::length(orientation.Position - orientation.LookAt);
    properties.LensRadius = 0.f;

    Camera = ProjectionCamera(orientation, properties, 0.f, 1.f);

    Background = glm::vec3(0.5f, 0.7f, 1.f);

    const auto& texture = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(glm::vec3(0.9f), glm::vec3(0.2f, 0.3f, 0.1f)));
    const auto& lambertianMaterial = std::make_shared<Lambertian>(glm::vec3(0.9f, 0.4f, 0.4f));
    const auto& dielectricMaterial = std::make_shared<Dielectric>(1.5f);
    const auto& metalMaterial = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.2f));

    std::vector<std::shared_ptr<GameObject>> objects;

    objects.push_back(std::make_shared<Sphere>(glm::vec3(0, -100.45f, 0), 100.f, texture));
    //objects.push_back(std::make_shared<MovingSphere>(glm::vec3(0.f, 0.1f, -3), 0.f, glm::vec3(0.f, -0.1f, -3), 1.f, 0.25f, lambertianMaterial));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(1.f, 0, -3), 0.5f, metalMaterial));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.f, -0.f, -3), 0.5f, lambertianMaterial));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.f, 0, -3), 0.5f, dielectricMaterial));
    Objects.Add(std::make_shared<BVHNode>(objects, 0, objects.size(), 0.f, 1.f));
}


void Scene::LoadPerlinNoiseScene()
{
    CameraOrientation orientation;
    orientation.Position = { 23.f, 3.f, 6 };
    orientation.LookAt = { 0, 2, 0 };
    orientation.Up = { 0, 1, 0 };

    CameraProps properties;
    properties.AspectRatio = 16.f / 9.f;
    properties.FOV = glm::radians(20.f);
    properties.FocusDist = glm::length(orientation.Position - orientation.LookAt);
    properties.LensRadius = 0.f;

    Camera = ProjectionCamera(orientation, properties, 0.f, 1.f);

    Background = glm::vec3(0);

    auto perlin = std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(10.f));
    auto earth = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../../../../RayTracing/assets/earthmap.jpg"));
    auto diffLight = std::make_shared<DiffuseLight>(glm::vec3(10.f));

    std::vector<std::shared_ptr<GameObject>> objects;

    objects.push_back(std::make_shared<Sphere>(glm::vec3(0, -1000.0f, 0), 1000.f, perlin));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.f, 1.f, 0), 1.f, earth));
    objects.push_back(std::make_shared<RectXY>(2.f, 6.f, 1.f, 3.f, -3.f, diffLight));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.f, 3.f, 0), 1.f, diffLight));

    Objects.Add(std::make_shared<BVHNode>(objects, 0, objects.size(), 0.f, 1.f));
}

void Scene::LoadCornellBoxScene()
{
    CameraOrientation orientation;
    orientation.Position = { 278, 278.f, -760 };
    orientation.LookAt = { 278, 278, 0 };
    orientation.Up = { 0, 1, 0 };

    CameraProps properties;
    properties.AspectRatio = 16.f / 9.f;
    properties.FOV = glm::radians(40.f);
    properties.FocusDist = glm::length(orientation.Position - orientation.LookAt);
    properties.LensRadius = 0.f;

    Camera = ProjectionCamera(orientation, properties, 0.f, 1.f);

    Background = glm::vec3(0.8f);

    GameObjectList objects;

    auto red = std::make_shared<Lambertian>(glm::vec3(0.65f, 0.05f, 0.05f));
    auto white = std::make_shared<Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
    auto green = std::make_shared<Lambertian>(glm::vec3(0.12f, 0.45f, 0.15f));
    auto light = std::make_shared<DiffuseLight>(glm::vec3(100));

    objects.Add(std::make_shared<RectYZ>(0, 555, 0, 555, 555, green));
    objects.Add(std::make_shared<RectYZ>(0, 555, 0, 555, 0, red));
    objects.Add(std::make_shared<RectXZ>(213, 343, 227, 332, 554, light));
    objects.Add(std::make_shared<RectXZ>(0, 555, 0, 555, 0, white));
    objects.Add(std::make_shared<RectXZ>(0, 555, 0, 555, 555, white));
    objects.Add(std::make_shared<RectXY>(0, 555, 0, 555, 555, white));

    Objects.Add(std::make_shared<BVHNode>(objects, 0.f, 1.f));
}

void Scene::LoadSceneSpheres()
{
    CameraOrientation orientation;
    orientation.Position = { 13, 2, 3 };
    orientation.LookAt = { 0, 0, 0 };
    orientation.Up = { 0, 1, 0 };

    CameraProps properties;
    properties.AspectRatio = 16.f / 9.f;
    properties.FOV = glm::radians(20.f);
    properties.FocusDist = 10.f;
    properties.LensRadius = 0.02f;

    Camera = ProjectionCamera(orientation, properties);

    Background = glm::vec3(0.5f, 0.7f, 1.f);

    auto ground_material = std::make_shared<Lambertian>(glm::vec3(0.5f));
    Objects.Add(std::make_shared<Sphere>(glm::vec3(0.f, -1000.f, 0.f), 1000.f, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++) 
        {
            float choose_mat = Random::Float();
            glm::vec3 center(a + 0.9f * Random::Float(), 0.2f, b + 0.9f * Random::Float());

            if ((center - glm::vec3(4.f, 0.2f, 0.f)).length() > 0.9f) 
            {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8f) 
                {
                    // diffuse
                    glm::vec3 albedo = Random::Vec3()* Random::Vec3();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    Objects.Add(std::make_shared<Sphere>(center, 0.2f, sphere_material));
                }
                else if (choose_mat < 0.95f)
                {
                    // metal
                    glm::vec3 albedo = Random::Vec3(0.5f, 1.f);
                    float fuzz = Random::Float(0.f, 0.5f);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    Objects.Add(std::make_shared<Sphere>(center, 0.2f, sphere_material));
                }
                else 
                {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5f);
                    Objects.Add(std::make_shared<Sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5f);
    Objects.Add(std::make_shared<Sphere>(glm::vec3(0.f, 1.f, 0.f), 1.f, material1));

    auto material2 = std::make_shared<Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f));
    Objects.Add(std::make_shared<Sphere>(glm::vec3(-4.f, 1.f, 0.f), 1.f, material2));

    auto material3 = std::make_shared<Metal>(glm::vec3(0.7f, 0.6f, 0.5f), 0.f);
    Objects.Add(std::make_shared<Sphere>(glm::vec3(4.f, 1.f, 0.f), 1.f, material3));
}

