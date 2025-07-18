#pragma once
#include <box2d/box2d.h>
#include <functional>
#include <memory>

class BodyPtr : public std::unique_ptr<b2BodyId,std::function<void(b2BodyId)>>
{
public:
	static BodyPtr Make( b2WorldId world,const b2BodyDef& bodyDef )
	{
		return { 
			&b2CreateBody(world, &bodyDef),
			[]( b2BodyId body ) 
			{ b2DestroyBody(body); }
		};
	}
	BodyPtr() = default;
private:
	BodyPtr( b2BodyId* p,std::function<void(b2BodyId)> f )
		:
		unique_ptr( p,f )
	{}
};