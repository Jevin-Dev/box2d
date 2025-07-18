#pragma once

#include <box2d/box2d.h>
#include "IndexedTriangleList.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Mat2.h"
#include "Colors.h"
#include "Pipeline.h"
#include "SolidEffect.h"
#include "BodyPtr.h"

class Boundaries
{
public:
	Boundaries( b2WorldId world,float size )
		:
		size( size )
	{
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_staticBody;
			bodyDef.position = { 0.0f,0.0f };
			bodyDef.userData = this;
			bodyId = b2CreateBody(world, &bodyDef);
		}
		{
			const float extents = 0.99f * size;
			const b2Vec2 vertices[] = {
				{ -extents,-extents },
				{  extents,-extents },
				{  extents, extents },
				{ -extents, extents }
			};
			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.isLoop = true;
			chainDef.count = 4;
			chainDef.points =  vertices;
			chainDef.materialCount = 1;
			b2SurfaceMaterial m = b2DefaultSurfaceMaterial();
			m.friction = 0.0f;
			m.restitution = 1.0f;
			chainDef.materials = &m;
			b2CreateChain(bodyId, &chainDef);
		}
	}
	float GetSize() const
	{
		return size;
	}
private:
	float size;
	b2BodyId bodyId;
};