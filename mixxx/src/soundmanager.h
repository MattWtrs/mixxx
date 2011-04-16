/***************************************************************************
                          soundmanager.h
                             -------------------
    begin                : Sun Aug 15, 2007
    copyright            : (C) 2007 Albert Santoni
    email                : gamegod \a\t users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "configobject.h"
#include "controlobject.h"
#include "controlobjectthreadmain.h"
#include "defs.h"
#ifdef __VINYLCONTROL__
#include "vinylcontrolproxy.h"
#endif
#include "soundmanagerconfig.h"
#include <QTimer>

class SoundDevice;
class EngineMaster;
class AudioOutput;
class AudioInput;
class AudioSource;
class AudioDestination;

#define MIXXX_PORTAUDIO_JACK_STRING "JACK Audio Connection Kit"
#define MIXXX_PORTAUDIO_ALSA_STRING "ALSA"
#define MIXXX_PORTAUDIO_OSS_STRING "OSS"
#define MIXXX_PORTAUDIO_ASIO_STRING "ASIO"
#define MIXXX_PORTAUDIO_DIRECTSOUND_STRING "Windows DirectSound"
#define MIXXX_PORTAUDIO_COREAUDIO_STRING "Core Audio"

class SoundManager : public QObject
{
    Q_OBJECT

    public:
        SoundManager(ConfigObject<ConfigValue> *pConfig, EngineMaster *_master);
        ~SoundManager();
        const EngineMaster* getEngine() const;
        QList<SoundDevice*> getDeviceList(QString filterAPI, bool bOutputDevices, bool bInputDevices);
        void closeDevices();
        void clearDeviceList();
        void queryDevices();
        int setupDevices();
        SoundDevice* getErrorDevice() const;
        QList<unsigned int> getSampleRates(QString api) const;
        QList<unsigned int> getSampleRates() const;
        QList<QString> getHostAPIList() const;
        SoundManagerConfig getConfig() const;
#ifdef __VINYLCONTROL__
        bool hasVinylInput(int deck);
        QList<VinylControlProxy*> getVinylControlProxies();
#endif
        int setConfig(SoundManagerConfig config);
        void checkConfig();
        QHash<AudioOutput, const CSAMPLE*>
            requestBuffer(QList<AudioOutput> outputs, unsigned long iFramesPerBuffer, SoundDevice*, double streamTime=0);
        void pushBuffer(QList<AudioInput> inputs, short *inputBuffer,
                        unsigned long iFramesPerBuffer, unsigned int iFrameSize);
        void registerOutput(AudioOutput output, const AudioSource *src);
        void registerInput(AudioInput input, AudioDestination *dest);
        QList<AudioOutput> registeredOutputs() const;
        QList<AudioInput> registeredInputs() const;
    signals:
        void devicesUpdated(); // emitted when all the pointers to SoundDevices go stale
        void devicesSetup(); // emitted when the sound devices have been set up
        void outputRegistered(AudioOutput output, const AudioSource *src);
        void inputRegistered(AudioInput input, AudioDestination *dest);
    public slots:
        void sync();
    private slots:
    	void slotInputPassthrough1(double);
    	void slotInputPassthrough2(double);
    private:
        void clearOperativeVariables();

        EngineMaster *m_pMaster;
        ConfigObject<ConfigValue> *m_pConfig;
        ControlObjectThreadMain *m_pControlObjectInputPassthrough1, *m_pControlObjectInputPassthrough2;
        ControlObjectThreadMain *m_pControlObjectVinylStatus1, *m_pControlObjectVinylStatus2;
        bool m_bPassthroughActive[2];
        QList<SoundDevice*> m_devices;
        QList<unsigned int> m_samplerates;
        QString m_hostAPI;
        QHash<AudioOutput, const CSAMPLE*> m_outputBuffers;
        QHash<AudioInput, short*> m_inputBuffers; /** Audio received from input */
        QHash<SoundDevice*, long> m_deviceFrameCount;   /** Sound card sync */
        SoundDevice* m_pClkRefDevice;  /** Sound card sync */
#ifdef __VINYLCONTROL__
        QList<VinylControlProxy*> m_vinylControl;
#endif
        unsigned int iNumDevicesOpenedForOutput;
        unsigned int iNumDevicesOpenedForInput;
        QMutex requestBufferMutex;
        SoundManagerConfig m_config;
        SoundDevice *m_pErrorDevice;
#ifdef __PORTAUDIO__
        bool m_paInitialized;
        unsigned int m_jackSampleRate;
#endif
        QHash<AudioOutput, const AudioSource*> m_registeredSources;
        QHash<AudioInput, AudioDestination*> m_registeredDestinations;
};

#endif
