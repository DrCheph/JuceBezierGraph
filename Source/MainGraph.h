/*
  ==============================================================================

    MainGraph.h
    Created: 29 Jul 2018 11:45:03am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MainGraph    : public Component
{
public:
    MainGraph();
    ~MainGraph();

    void paint (Graphics&) override;
    void resized() override;
    
 //==============================================================================
    
    //gives our nodes some data to start with
    void initializeNodes();
    
    //mouse event handleing functions that must be overriden
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    
private:
    
    //used to update the nodes on start
    bool update = true;
    

    //two controle points for the cubic equation
    Point<int> node1;
    Point<int> node2;
    
    //true if the user is currently dragging a node with their mouse otherwise false
    bool DragingNode1;
    bool DragingNode2;
    
    //the path that will be the line drawen on the graph
    Path path;
    
    //required to draw a path without closing it, must be initialized
    PathStrokeType pathStroke;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainGraph)
};
