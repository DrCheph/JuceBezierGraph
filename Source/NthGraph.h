/*
  ==============================================================================

    NthGraph.h
    Created: 29 Jul 2018 4:03:29pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//remember to include vector
#include <vector>

//==============================================================================
/*
*/


class NthGraph    : public Component
{
public:
    NthGraph();
    ~NthGraph();

    void paint (Graphics&) override;
    void resized() override;
//==============================================================================
    
    //gives our nodes some data to start with
    void initializeNodes();
    
    //mouse event handleing functions that must be overriden
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    
    //function to draw a muli point Bezier Curve
    //note this is not a nth order Bezier curve but a bunch of quadratic Bezier curves stiched together
    void multipointBezierCurve(const std::vector<Point<int>> &cp, Path &path);
    

private:
    
    //used to update the nodes on start
    bool update = true;
    
    //controle points stored in a vector
    std::vector<Point<int>> nodes;
    
    //the current node that is being dragged, set to -1 if no nodes are being draged
    int draggingNodeNum;
    
    //the path that will be the line drawen on the graph
    Path path;
    
    //required to draw a path without closing it, must be initialized
    PathStrokeType pathStroke;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NthGraph)
};
