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
#include "Boundaries.h"
#include <random>

class Box
{
public:
	class ColorTrait
	{
	public:
		virtual ~ColorTrait() = default;
		virtual Color GetColor() const = 0;
		virtual std::unique_ptr<ColorTrait> Clone() const = 0;
	};
public:
	static std::unique_ptr<Box> Box::Spawn( float size,const Boundaries& bounds,b2WorldId world,std::mt19937& rng );
	Box( std::unique_ptr<ColorTrait> pColorTrait, b2WorldId world,const Vec2& pos,
		float size = 1.0f,b2Rot angle = b2MakeRot(0.0f),Vec2 linVel = {0.0f,0.0f},float angVel = 0.0f )
		:
		pColorTrait( std::move( pColorTrait ) ),
		size( size )
	{
		Init();
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = b2Vec2( pos );
			bodyDef.linearVelocity = b2Vec2( linVel );
			bodyDef.angularVelocity = angVel;
			bodyDef.rotation = angle;
			bodyDef.userData = this;
			bodyId = b2CreateBody( world, &bodyDef );
		}
		{
			b2Polygon dynamicBox = b2MakeBox(size, size);
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.density = 1.0f;
			shapeDef.material.friction = 0.0f;
			shapeDef.material.restitution = 1.0f;
			b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
		}
	}
	void Draw( Pipeline<SolidEffect>& pepe ) const
	{
		pepe.effect.vs.BindTranslation( GetPosition() );
		pepe.effect.vs.BindRotation( Mat2::Rotation( GetAngle() ) * Mat2::Scaling( GetSize() ) );
		pepe.effect.ps.BindColor( GetColorTrait().GetColor() );
		pepe.Draw( model );
	}
	void ApplyLinearImpulse( const Vec2& impulse )
	{
		b2Body_ApplyLinearImpulse( bodyId, (b2Vec2)impulse,(b2Vec2)GetPosition(),true );
	}
	void ApplyAngularImpulse( float impulse )
	{
		b2Body_ApplyAngularImpulse( bodyId, impulse,true );
	}
	b2Rot GetAngle() const
	{
		return b2Body_GetRotation(bodyId);
	}
	Vec2 GetPosition() const
	{
		return (Vec2)b2Body_GetPosition(bodyId);
	}
	float GetAngularVelocity() const
	{
		return b2Body_GetAngularVelocity(bodyId);
	}
	Vec2 GetVelocity() const
	{
		return (Vec2)b2Body_GetLinearVelocity(bodyId);
	}
	float GetSize() const
	{
		return size;
	}
	const ColorTrait& GetColorTrait() const
	{
		return *pColorTrait;
	}
	void MarkForDeath()
	{
		isDying = true;
	}
	bool IsDying() const
	{
		return isDying;
	}
	void AssumeColorTrait( std::unique_ptr<ColorTrait> pct )
	{
		pColorTrait = std::move( pct );
	}
	std::vector<std::unique_ptr<Box>> Box::Split( b2WorldId& world );
private:
	static void Init()
	{
		if( model.indices.size() == 0 )
		{
			model.vertices = { { -1.0f,-1.0 },{ 1.0f,-1.0 },{ -1.0f,1.0 },{ 1.0f,1.0 } };
			model.indices = { 0,1,2, 1,2,3 };
		}
	}
private:
	static IndexedTriangleList<Vec2> model;
	float size;
	b2BodyId bodyId;
	std::unique_ptr<ColorTrait> pColorTrait;
	bool isDying = false;
};