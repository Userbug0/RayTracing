#include "GameObject.h"


bool GameObjectList::Intersect(const Ray& ray, float minLength, float maxLength, HitRecord& record) const
{
	record.Distance = maxLength;
	bool hitAnything = false;

	for (size_t i = 0; i < m_Objects.size(); ++i)
	{
		if (m_Objects[i]->Intersect(ray, minLength, record.Distance, record))
		{
			hitAnything = true;
		}
	}

	return hitAnything;
}

bool GameObjectList::ConstructAABB(float time0, float time1, AABB& outputBox) const
{
	AABB result;
	bool firstBox = true;

	for (const auto& object : m_Objects)
	{
		if (!object->ConstructAABB(time0, time1, result))
			return false;

		firstBox ? outputBox = result : outputBox.Merge(result);
		firstBox = false;
	}

	return true;
}
