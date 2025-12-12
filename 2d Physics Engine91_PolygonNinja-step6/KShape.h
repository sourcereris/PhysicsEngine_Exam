/*
	Windows porting in 2021: jintaeks@dongseo.ac.kr
*/

#ifndef SHAPE_H
#define SHAPE_H

#include "KRigidbody.h"
#include "KMath.h"
#include <memory>

struct KRigidbody;
struct KShape;
typedef std::shared_ptr<KShape>	KShapePtr;

struct KAABB
{
	KVector2 min;
	KVector2 max;
	
	static bool Overlaps(const KAABB& a, const KAABB& b)
	{
		if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
		if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
		return true;
	}
};

struct KShape
{
	enum Type
	{
		eCircle,
		ePoly,
		eCount
	};

public:
	KShape() : body(nullptr)
	{
		r = Random(0.2f, 0.8f);
		g = Random(0.2f, 0.8f);
		b = Random(0.2f, 0.8f);
	}

	KAABB m_aabb;
	virtual void ComputeAABB() = 0;

	virtual void Initialize() = 0;
	virtual void ComputeMass(float density) = 0;
	virtual void SetRotation(float radians) = 0;
	virtual Type GetType() const = 0;
	void SetPosition(const KVector2& pos_) { position = pos_; }

public:
	std::shared_ptr<KRigidbody> body;
	KMatrix2 rotation; // Orientation matrix from model to world
	KVector2 position;
	// Store a color in RGB format
	float r, g, b;
};


#endif // SHAPE_H
