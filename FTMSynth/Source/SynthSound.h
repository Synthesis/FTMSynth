/*
  ==============================================================================

    SynthSound.h
    Created: 10 Oct 2019 10:03:03am
    Author:  Lily H

  ==============================================================================
*/

#pragma once

class SynthSound : public SynthesiserSound
{
public:
    bool appliesToNote(int /*midinotes*/)
    {
        return true;
    };
    bool appliesToChannel(int /*midichannel*/)
    {
        return true;
    };
};
