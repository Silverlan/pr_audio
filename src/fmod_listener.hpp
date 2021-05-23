/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FMOD_LISTENER_HPP__
#define __FMOD_LISTENER_HPP__

#include <alsound_listener.hpp>

namespace al
{
	class FMSoundSystem;
	class FMListener
		: public IListener
	{
	public:
		virtual void SetGain(float gain) override;
		virtual void SetPosition(const Vector3 &pos) override;
		virtual void SetVelocity(const Vector3 &vel) override;
		virtual void SetOrientation(const Vector3 &at,const Vector3 &up) override;
	protected:
		FMListener(al::ISoundSystem &system);
		virtual void DoSetMetersPerUnit(float mu) override;
		friend FMSoundSystem;
	};
};

#endif
