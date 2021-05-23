/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FMOD_SOUND_SOURCE_HPP__
#define __FMOD_SOUND_SOURCE_HPP__

#include <alsound_source.hpp>

namespace FMOD
{
	class Channel;
};
namespace al
{
	class FMSoundChannel
		: public ISoundChannel
	{
	public:
		FMSoundChannel(ISoundSystem &system,ISoundBuffer &buffer);
		FMSoundChannel(ISoundSystem &system,Decoder &decoder);
		void SetSource(FMOD::Channel *source);
		void SetFMOD3DAttributesEffective(bool b);

		virtual void Update() override;

		virtual void Play() override;
		virtual void Stop() override;
		virtual void Pause() override;
		virtual void Resume() override;
		virtual bool IsPlaying() const override;
		virtual bool IsPaused() const override;
		virtual void SetPriority(uint32_t priority) override;
		virtual uint32_t GetPriority() const override;
		virtual void SetFrameOffset(uint64_t offset) override;
		virtual uint64_t GetFrameOffset(uint64_t *latency=nullptr) const override;
		virtual void SetLooping(bool bLoop) override;
		virtual bool IsLooping() const override;

		virtual void SetPitch(float pitch) override;
		virtual float GetPitch() const override;

		virtual void SetGain(float gain) override;
		virtual float GetGain() const override;

		virtual void SetGainRange(float minGain,float maxGain) override;
		virtual std::pair<float,float> GetGainRange() const override;
		virtual float GetMinGain() const override;
		virtual float GetMaxGain() const override;
		virtual void SetDistanceRange(float refDist,float maxDist) override;
		virtual std::pair<float,float> GetDistanceRange() const override;
		virtual void SetPosition(const Vector3 &pos) override;
		virtual Vector3 GetPosition() const override;
		virtual void SetVelocity(const Vector3 &vel) override;
		virtual Vector3 GetVelocity() const override;

		virtual void SetDirection(const Vector3 &dir) override;
		virtual Vector3 GetDirection() const override;

		virtual void SetOrientation(const Vector3 &at,const Vector3 &up) override;
		virtual std::pair<Vector3,Vector3> GetOrientation() const override;

		virtual void SetConeAngles(float inner,float outer) override;
		virtual std::pair<float,float> GetConeAngles() const override;
		virtual void SetOuterConeGains(float gain,float gainHF=1.f) override;
		virtual std::pair<float,float> GetOuterConeGains() const override;
		virtual float GetOuterConeGain() const override;
		virtual float GetOuterConeGainHF() const override;

		virtual void SetRolloffFactors(float factor,float roomFactor=0.f) override;
		virtual std::pair<float,float> GetRolloffFactors() const override;
		virtual float GetRolloffFactor() const override;
		virtual float GetRoomRolloffFactor() const override;

		virtual void SetDopplerFactor(float factor) override;
		virtual float GetDopplerFactor() const override;

		virtual void SetRelative(bool bRelative) override;
		virtual bool IsRelative() const override;

		virtual void SetRadius(float radius) override;
		virtual float GetRadius() const override;

		virtual void SetStereoAngles(float leftAngle,float rightAngle) override;
		virtual std::pair<float,float> GetStereoAngles() const override;
		virtual void SetAirAbsorptionFactor(float factor) override;
		virtual float GetAirAbsorptionFactor() const override;

		virtual void SetGainAuto(bool directHF,bool send,bool sendHF) override;
		virtual std::tuple<bool,bool,bool> GetGainAuto() const override;
		virtual bool GetDirectGainHFAuto() const override;
		virtual bool GetSendGainAuto() const override;
		virtual bool GetSendGainHFAuto() const override;

		virtual void SetDirectFilter(const EffectParams &params) override {}
		virtual void SetEffectParameters(uint32_t slotId,const EffectParams &params) override {}

		const FMOD::Channel *GetInternalSource() const;
		FMOD::Channel *GetInternalSource();
	protected:
		virtual void DoAddEffect(IAuxiliaryEffectSlot &slot,uint32_t slotId,const EffectParams &params) override {}
		virtual void DoRemoveInternalEffect(uint32_t slotId) override {}
		virtual void DoRemoveEffect(uint32_t slotId) override {}
		struct SoundSourceData
		{
			uint64_t offset = 0ull;
			uint32_t priority = 0u;
			bool looping = false;
			float pitch = 1.f;
			float gain = 1.f;
			float minGain = 0.f;
			float maxGain = 1.f;
			std::pair<float,float> distanceRange = {1.f,10'000.f};
			Vector3 position = {};
			Vector3 velocity = {};
			std::pair<float,float> coneAngles = {360.f,360.f};
			float dopplerFactor = 1.f;
			bool relativeToListener = false;
		};
		void UpdateMode();
		bool Is3D() const;
		bool Is2D() const;
		bool CheckResultAndUpdateValidity(uint32_t result) const;
		void InvalidateSource() const;
		bool InitializeChannel();
		SoundSourceData m_soundSourceData = {};
	private:
		mutable FMOD::Channel *m_source = nullptr;
	};
};

#endif
