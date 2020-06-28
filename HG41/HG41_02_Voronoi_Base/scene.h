#pragma once

#include <list>
#include "main.h"

#include "game_object.h"

#include "camera.h"
#include "model.h"
#include "polygon.h"
#include "polygon1.h"

#include "Timer.h"

class CScene
{
protected:
	std::list<CGameObject*>	m_GameObject;

public:
	CScene(){}
	virtual ~CScene(){}


	virtual void Init()
	{
		AddGameObject<CCamera>();
		//AddGameObject<CModel>();
		AddGameObject<CPolygon>();
		AddGameObject<CPolygon1>();
	}

	virtual void Uninit()
	{
		for (CGameObject* object : m_GameObject)
		{
			object->Uninit();
			delete object;
		}

		m_GameObject.clear();
	}


	virtual void Update()
	{
		const auto dt = timer.Mark() * speed_factor;

		for( CGameObject* object : m_GameObject )
			object->Update(dt);
	}


	virtual void Draw()
	{
		for (CGameObject* object : m_GameObject)
			object->Draw();
	}


	template <typename T>
	T* AddGameObject()
	{
		T* gameObject = new T();
		gameObject->Init();
		m_GameObject.push_back( gameObject );

		return gameObject;
	}

private:
	Timer timer;
	float speed_factor = 1.0f;
};