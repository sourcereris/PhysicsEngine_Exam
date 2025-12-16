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

void KParticleSystem::Draw(HDC hdc)
{
	// --- GDI BATCH SETUP ---
	// Use NULL_PEN (no border) and DC_BRUSH (solid fill) to minimize object creation overhead.
	// This allows to just change the DC brush color per particle.
	HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));

	for (size_t i = 0; i < m_particles.size(); i++)
	{
		KParticle& part = *m_particles[i];

		// --- COORDINATE TRANSFORMATION ---
		// Manually convert to screen space to bypass the heavier KVectorUtil::DrawLine logic.
		KVector2 screenPos = KVectorUtil::WorldToScreen(part.GetPosition());

		// --- LIFETIME VISUALS ---
		// Simulate "fading out" by reducing size based on age ratio, as GDI alpha blending is expensive.
		double ratio = part.GetAge() / part.GetLifetime();
		int size = 2; // Base radius

		if (ratio > 0.5) size = 1; // Shrink
		if (ratio > 0.8) size = 0; // Disappear

		if (size > 0)
		{
			SetDCBrushColor(hdc, part.GetColor());

			// Draw primitive (Rectangle is faster than Ellipse)
			Rectangle(hdc,
				(int)screenPos.x - size, (int)screenPos.y - size,
				(int)screenPos.x + size, (int)screenPos.y + size);
		}
	}

	// Restore previous GDI state
	SelectObject(hdc, oldBrush);
	SelectObject(hdc, oldPen);
}