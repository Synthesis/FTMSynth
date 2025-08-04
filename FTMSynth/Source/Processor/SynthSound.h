/*
  ==============================================================================

    SynthSound.h
    Created: 10 Oct 2019 10:03:03am
    Author:  Lily H

  ==============================================================================

    This file is part of FTMSynth.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

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
