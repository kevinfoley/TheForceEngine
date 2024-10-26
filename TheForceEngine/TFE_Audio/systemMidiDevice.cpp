#include <algorithm>
#include <cstring>
#include <SDL_mutex.h>
#include "midi.h"

#include "systemMidiDevice.h"
#ifdef __linux__
 #include <rtmidi/RtMidi.h>
#else
 #include "RtMidi.h"
#endif
#include <TFE_System/system.h>

#ifdef _WIN32
// Windows library required to access the midi device(s).
#pragma comment( lib, "winmm.lib" )
#endif

//This system uses "RtMidi" as the low level, cross platform interface to midi devices.
//https://www.music.mcgill.ca/~gary/rtmidi/

namespace TFE_Audio
{
	static const char* c_SystemMidi_Name = "System Midi";
	enum
	{
		SMID_DRUM_CHANNEL = 9,
	};

	void midiErrorCallback(RtMidiError::Type, const std::string&, void*);

	SystemMidiDevice::SystemMidiDevice()
	{
		m_outputId = 0;		// "Disabled" device.
		m_midiout = new RtMidiOut();
		m_midiout->setErrorCallback(midiErrorCallback);
		portLock = SDL_CreateMutex();

		getOutputCount();
	}

	SystemMidiDevice::~SystemMidiDevice()
	{
		exit();
		SDL_DestroyMutex(portLock);
		portLock = nullptr;
	}

	void SystemMidiDevice::exit()
	{
		if (m_outputId > 0) { m_midiout->closePort(); }
		if (m_midiout)
		{
			delete m_midiout;
			m_midiout = nullptr;
		}
		m_outputId = 0;
	}

	const char* SystemMidiDevice::getName()
	{
		return c_SystemMidi_Name;
	}

	void SystemMidiDevice::message(const u8* msg, u32 len)
	{
		if (m_outputId > 0)
		{
			// this mutex is uncontended except for a short time
			// after a midi device switch is done in the settings.
			SDL_LockMutex(portLock);
			m_midiout->sendMessage(msg, (size_t)len);
			SDL_UnlockMutex(portLock);
		}
	}

	void SystemMidiDevice::message(u8 type, u8 arg1, u8 arg2)
	{
		const u8 msg[3] = { type, arg1, arg2 };
		message(msg, 3);
	}


	void SystemMidiDevice::noteAllOff()
	{
		for (u32 c = 0; c < MIDI_CHANNEL_COUNT; c++)
		{
			message(MID_CONTROL_CHANGE + c, MID_ALL_NOTES_OFF, 0);
		}
	}

	void SystemMidiDevice::setVolume(f32 volume)
	{
		// No-op.
	}

	u32 SystemMidiDevice::getOutputCount()
	{
		m_outputs.clear();
		if (m_midiout)
		{
			u32 count = m_midiout->getPortCount();

			m_outputs.push_back(count ? "Disabled" : "Disabled: no ports found");

			for (u32 i = 0; i < count; i++)
			{
				m_outputs.push_back(m_midiout->getPortName(i));
			}
		} else {
			m_outputs.push_back("Disabled: internal MIDI error");
		}
		return (u32)m_outputs.size();
	}

	void SystemMidiDevice::getOutputName(s32 index, char* buffer, u32 maxLength)
	{
		if (index < 0 || index >= (s32)getOutputCount()) { return; }

		const std::string& name = m_outputs[index];
		const u32 copyLength = std::min((u32)name.length(), maxLength - 1);
		strncpy(buffer, name.c_str(), copyLength);
		buffer[copyLength] = 0;
	}

	bool SystemMidiDevice::selectOutput(s32 index)
	{
		if (index < 0 || index >= (s32)getOutputCount())
		{
			index = 0;	// "disabled" device
		}
		if (index != m_outputId && m_midiout)
		{
			noteAllOff();
			if (m_outputId > 0)
				m_midiout->closePort();
			if (index > 0)	// real Device
			{
				m_midiout->openPort(index - 1);
				for (s32 i = 0; i < MIDI_CHANNEL_COUNT; i++)
				{
					u8 msg[2] = { u8(MID_PROGRAM_CHANGE | i), 0 };
					message(msg, 2);
				}
			}
		}
		m_outputId = index;
		return true;
	}

	s32 SystemMidiDevice::getActiveOutput(void)
	{
		return m_outputId;
	}

	void midiErrorCallback(RtMidiError::Type type, const std::string &errorText, void *userData)
	{
		TFE_System::logWrite(LOG_ERROR, "Midi Device", "%s", errorText.c_str());
	}
}
