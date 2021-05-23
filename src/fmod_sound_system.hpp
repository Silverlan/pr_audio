/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <alsoundsystem.hpp>

namespace FMOD
{
	class System;
	namespace Studio
	{
		class System;
	};
};
namespace al
{
	void check_result(uint32_t r);
	class FMSoundSystem
		: public ISoundSystem
	{
	public:
		static std::shared_ptr<FMSoundSystem> Create(const std::string &deviceName,float metersPerUnit=1.f);
		static std::shared_ptr<FMSoundSystem> Create(float metersPerUnit=1.f);
		virtual void OnRelease() override;

		virtual void Update() override;

		virtual IAuxiliaryEffectSlot *CreateAuxiliaryEffectSlot() override;
		
		virtual PDecoder CreateDecoder(const std::string &path,bool bConvertToMono=false) override;
		virtual PEffect CreateEffect() override;
		virtual bool IsSupported(ChannelConfig channels,SampleType type) const override;

		virtual float GetDopplerFactor() const override;
		virtual void SetDopplerFactor(float factor) override;

		virtual float GetSpeedOfSound() const override;
		virtual void SetSpeedOfSound(float speed) override;

		virtual DistanceModel GetDistanceModel() const override;
		virtual void SetDistanceModel(DistanceModel mdl) override;
		
		virtual std::string GetDeviceName() const override;
		virtual void PauseDeviceDSP() override;
		virtual void ResumeDeviceDSP() override;

		virtual std::vector<std::string> GetDevices() override {return {"fmod"};}
		virtual std::string GetDefaultDeviceName() override {return "fmod";}

		virtual uint32_t GetMaxAuxiliaryEffectsPerSource() const override;

		// HRTF
		virtual std::vector<std::string> GetHRTFNames() const override;
		virtual std::string GetCurrentHRTF() const override;
		virtual bool IsHRTFEnabled() const override;
		virtual void SetHRTF(uint32_t id) override;
		virtual void DisableHRTF() override;

		const FMOD::Studio::System &GetFMODSystem() const;
		FMOD::Studio::System &GetFMODSystem();
		const FMOD::System &GetFMODLowLevelSystem() const;
		FMOD::System &GetFMODLowLevelSystem();
	private:
		FMSoundSystem(const std::shared_ptr<FMOD::Studio::System> &fmSystem,FMOD::System &lowLevelSystem,float metersPerUnit);
		virtual PSoundChannel CreateChannel(ISoundBuffer &buffer) override;
		virtual PSoundChannel CreateChannel(Decoder &decoder) override;
		virtual ISoundBuffer *DoLoadSound(const std::string &path,bool bConvertToMono=false,bool bAsync=true) override;
		virtual std::unique_ptr<IListener> CreateListener() override;
		std::shared_ptr<FMOD::Studio::System> m_fmSystem = nullptr;
		FMOD::System &m_fmLowLevelSystem;
	};
};
