/*
  ==============================================================================

    MidiConfigView.cpp
    Created: 4 Oct 2024 12:03:56am
    Author:  Loïc J

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

#include "MidiConfigView.h"

//==============================================================================
MidiConfigView::MidiConfigView(FTMSynthAudioProcessor& p)
    : processor(p)
{
    setSize(640, 400);
    setInterceptsMouseClicks(false, true);
}

MidiConfigView::~MidiConfigView()
{
}

void MidiConfigView::paint(juce::Graphics&)
{
}

void MidiConfigView::resized()
{
    setSize(640, 400);
}
