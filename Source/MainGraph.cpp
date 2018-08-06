/*
  ==============================================================================

    MainGraph.cpp
    Created: 29 Jul 2018 11:45:03am
    Author:  Sam Ferguson
 
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

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainGraph.h"

//==============================================================================
MainGraph::MainGraph() : pathStroke(3.f, PathStrokeType::curved, PathStrokeType::butt)
{

}

MainGraph::~MainGraph()
{
}

void MainGraph::initializeNodes()
{
    //gives our nodes some data to start with, can't be done in the constructor
    //because getWidth and getHight won't work untill after construction
    node1.setXY(int(getWidth()/4), int(getHeight()/4));
    node2.setXY(int(getWidth()/4) * 3, int(getHeight()/4) * 3);
}
void MainGraph::mouseDown(const MouseEvent& e)
{

    //compaire the distance between where the user clicked and the two nodes
    if(e.getMouseDownPosition().getDistanceFrom(node1) <= e.getMouseDownPosition().getDistanceFrom(node2))
    {
        // the node gets repositioned to where the user clicked and is marked as being draged
        node1 = e.getMouseDownPosition();
        DragingNode1 = true;
    }
    else
    {
        node2 = e.getMouseDownPosition();
        DragingNode2 = true;
    }
    
    //repaint the graph
    repaint();
    
    
}
void MainGraph::mouseDrag(const MouseEvent& e)
{
    //check which node is being draged
    if(DragingNode1)
    {
        //assign the node's position to the current position
        node1 = e.getPosition();
        
        //check to make sure the node stays in the constrains of the graph
        if(node1.x <= 0)
        {
            node1.x = 0;
        }
        else if(node1.x >= getWidth())
        {
            node1.x = getWidth();
        }
        if(node1.y <= 0)
        {
            node1.y = 0;
        }
        else if(node1.y >= getHeight())
        {
            node1.y = getHeight();
        }
        
        //repaint the graph
        repaint();
    }
    else if(DragingNode2)
    {

        node2 = e.getPosition();
        
        if(node2.x <= 0)
        {
            node2.x = 0;
        }
        else if(node2.x >= getWidth())
        {
            node2.x = getWidth();
        }
        if(node2.y <= 0)
        {
            node2.y = 0;
        }
        else if(node2.y >= getHeight())
        {
            node2.y = getHeight();
        }
        
        repaint();
    }
    
    
}
void MainGraph::mouseUp(const MouseEvent& e)
{
    //when the user "unclicks" dragging has stoped
    DragingNode1 = false;
    DragingNode2 = false;
}
void MainGraph::paint (Graphics& g)
{
    
    //one time update to give the nodes some positon on start
    if(update)
    {
        initializeNodes();
        update = false;
    }
    
    //draw the background of the graph white
    g.setColour (Colours::white);
    g.fillRect(getLocalBounds());
    
   
    //drawing dashed lines coneting the nodes
    //==============================================================================
    
    g.setColour (Colours::grey);

    //an array that defines how many pixles each line segment and break will be
    float dashArray[4]{4,4,4,4};
    
    //make a line object to give to the graphics object
    Line<float> dashedLine(0, float(getHeight()), float(node1.x), float(node1.y));
    
    //draw the first line
    g.drawDashedLine(dashedLine, dashArray, 4, 1.f, 0);
    
    //redefine the line
    dashedLine.setStart(float(node1.x),float(node1.y));
    dashedLine.setEnd(float(node2.x),float(node2.y));
    //draw it again
    g.drawDashedLine(dashedLine, dashArray, 4, 1.f, 0);
    //redifine one last time
    dashedLine.setStart(float(node2.x),float(node2.y));
    dashedLine.setEnd(float(getWidth()), 0);
    //and draw it
    g.drawDashedLine(dashedLine, dashArray, 4, 1.f, 0);
    
    
    //draw our cubic line
    // ==============================================================================
    g.setColour (Colours::black);
    
    //clear the old one and reset start point
    path.clear();
    path.startNewSubPath(0, getHeight());
    
    //cubic function
    path.cubicTo(float(node1.x), float(node1.y), float(node2.x), float(node2.y), getWidth(), 0);
    
    //take a little break to extract data
    
//==============================================================================
    //in order to extract the x and y information out of our cubic line first a closed path must be made
    
    //coppy the original path
    Path pathCopy(path);
    
    //close the path by going to the top right, then top left then back to the start
    pathCopy.lineTo(float(getWidth()), 0);
    pathCopy.lineTo(0, 0);
    pathCopy.closeSubPath();
    
    //make a line to measure against an axis
    Line<float> line;
    
    //set the line at some x position greater than 0 then draw it all the wat to the top + 1
    //remember the top it 0 since the pixels count up from the top of the graph down
    line.setStart(float(getWidth()/2), float(getHeight()));
    line.setEnd(float(getWidth()/2), 1.f);
    
    //clip the line when it intersects with the closed path
    line = pathCopy.getClippedLine(line, true);
    
    //extract the y value at the entered x with line.getEndY()
    //note this will be upsidown since the pixles count up from the top of the graph down
    
    //DBG(line.getEndY());
    
//==============================================================================
    
    //create a stroked path from the cubic path we made
    
    pathStroke.createStrokedPath(path, path);
    
    //fill that path
    g.fillPath(path);
    
    //draw some Ellipses to represent the nodes
    g.setColour (Colours::blue);
    g.fillEllipse(node1.x - 5, node1.y - 5, 10, 10);
    g.setColour (Colours::red);
    g.fillEllipse(node2.x - 5, node2.y - 5, 10, 10);

}

void MainGraph::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
