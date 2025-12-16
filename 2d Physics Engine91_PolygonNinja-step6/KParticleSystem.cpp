#include "KParticleSystem.h"
#include "KVectorUtil.h"
#include <algorithm>
#include "KRgb.h"

void KParticleSystem::Initialize(KParticleSystemInitParam param)
{
	param.gravity;
	param.wind;
	param.initialNumParticle;
	param.maximumNumParticle;
	param.color;
	param.defaultLifetime;
	param.isRegenerate;
	param.initCallback;
	param.afterUpdateCallback;
	param.generateParticleCallback;

	m_Regenerate = param.isRegenerate;
	m_Position = param.position;
	m_maximumNumParticle = param.maximumNumParticle;;
	m_Color = param.color;;
	m_defaultLifetimeOfParticle = param.defaultLifetime;;
	m_initCallback = param.initCallback;
	m_afterUpdateCallback = param.afterUpdateCallback;
	m_generateParticleCallback = param.generateParticleCallback;

	m_spParticleSystemData.reset(new KParticleSystemData());
	m_particles.reserve(m_maximumNumParticle);
	m_spParticleSystemData->SetGravity(param.gravity);
	m_spParticleSystemData->SetWind(param.wind);
	m_spParticleSystemData->SetPosition(param.position);

	if (m_initCallback) {
		m_initCallback(__super::shared_from_this());
	}
}

void KParticleSystem::AddParticle()
{
	if (m_generateParticleCallback) {
		KParticlePtr particle = m_generateParticleCallback(shared_from_this());
		m_particles.push_back(particle);
	}
}

bool KParticleSystem::Update(float fElapsedTime)
{
	std::vector<KParticlePtr> particles;
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		KParticlePtr particle = m_particles[i];
		particle->Update(fElapsedTime);
		if (particle->IsAlive()) {
			particles.push_back(particle);
		}
	}
	m_particles = particles;
	bool ret = false;
	if (m_afterUpdateCallback) {
		ret = m_afterUpdateCallback(shared_from_this());
	}

	return false;
}

// In KParticleSystem.cpp

void KParticleSystem::Draw(HDC hdc)
{
	// Optimization: Cache the coordinate system once
	// (Instead of recalculating matrices for every single pixel)

	// We will draw simple filled rectangles/dots which is MUCH faster than lines
	HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN)); // No borders
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(DC_BRUSH)); // Solid fill

	for (size_t i = 0; i < m_particles.size(); i++)
	{
		KParticle& part = *m_particles[i];

		// 1. Convert World Position to Screen Position manually
		// (This skips the overhead of KVectorUtil::DrawLine)
		KVector2 screenPos = KVectorUtil::WorldToScreen(part.GetPosition());

		// 2. Calculate fade/size
		// Simple visual: Fade alpha is hard in GDI, so we simulate fade by shrinking
		double ratio = part.GetAge() / part.GetLifetime();
		int size = 2; // Default size (radius in pixels)

		if (ratio > 0.5) size = 1; // Shrink as it dies
		if (ratio > 0.8) size = 0; // Disappear near end

		if (size > 0)
		{
			// 3. Set Color
			SetDCBrushColor(hdc, part.GetColor());

			// 4. Draw Fast Rectangle (or Ellipse)
			// Ellipse is slightly slower than Rectangle, but much faster than 10 lines.
			// For sparks, rectangles look fine.
			Rectangle(hdc,
				(int)screenPos.x - size, (int)screenPos.y - size,
				(int)screenPos.x + size, (int)screenPos.y + size);
		}
	}

	// Restore GDI objects
	SelectObject(hdc, oldBrush);
	SelectObject(hdc, oldPen);
}
