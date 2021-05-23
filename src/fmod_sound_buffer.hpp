/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FMOD_SOUND_BUFFER_HPP__
#define __FMOD_SOUND_BUFFER_HPP__

#include <alsound_buffer.hpp>

namespace FMOD
{
	class System;
	class Sound;
	namespace Studio
	{
		class System;
	};
};
namespace al
{
	class FMSoundBuffer
		: public ISoundBuffer
	{
	public:
		FMSoundBuffer(FMOD::System &system,const std::shared_ptr<FMOD::Sound> &sound);
		virtual ~FMSoundBuffer() override;

		virtual bool IsReady() const override;

		virtual uint32_t GetSize() const override;
		virtual void SetLoopFramePoints(uint32_t start,uint32_t end) override;
		virtual void SetLoopTimePoints(float tStart,float tEnd) override;

		virtual uint32_t GetFrequency() const override;
		virtual ChannelConfig GetChannelConfig() const override;
		virtual SampleType GetSampleType() const override;
		virtual uint64_t GetLength() const override;
		virtual std::pair<uint64_t,uint64_t> GetLoopFramePoints() const override;

		virtual std::string GetName() const override;
		virtual bool IsInUse() const override;

		const FMOD::Sound *GetFMODSound() const;
		FMOD::Sound *GetFMODSound();
	private:
		FMOD::System &m_fmSystem;
		std::shared_ptr<FMOD::Sound> m_fmSound = nullptr;
	};
};

#endif
