/*
  ==============================================================================

    NthGraph.h
    Created: 29 Jul 2018 4:03:29pm
    Author:  Sam Ferguson
 
This is a framework for a GUI element that alows the user to control a bunch of curves witha control node
 
written with the JUCE framework
 
 This file is part of JuceBezierGraph.
 
 Foobar is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Foobar is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//remember to include vector
#include <vector>

//==============================================================================


class NthGraph    : public Component, public Timer
{
public:
    NthGraph();
    ~NthGraph();

    void paint (Graphics&) override;
    void resized() override;
//==============================================================================
    
    //gives our nodes some data to start with
    void initializeNodes();
    
    //used to throttle the mouse drag method
    void timerCallback() override;
    
    //mouse event handleing functions that must be overriden
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    
    //function to draw a muli point Bezier Curve
    //note this is not a nth order Bezier curve but a bunch of quadratic Bezier curves stiched together
    void multipointBezierCurve();
    

private:
    
    //used to update the nodes on start
    bool update = true;
    
    //used to update the path if nessisary
    bool runPathing = false;
    
    //used to throttle the mouse drag method
    bool frame = false;
    
    //controle points stored in a vector
    std::vector<Point<int>> nodes;
    
    //the current node that is being dragged, set to -1 if no nodes are being draged
    int draggingNodeNum;
    
    //array of paths that will be drawn, these start at 1 not 0
    OwnedArray<Path> quadPaths;
    
    //path for copying and sampling 
    Path path;
    
    //required to draw a path without closing it, must be initialized
    PathStrokeType pathStroke;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NthGraph)
};
