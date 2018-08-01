/*
  ==============================================================================

    NthGraph.cpp
    Created: 29 Jul 2018 4:03:29pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NthGraph.h"

//==============================================================================
NthGraph::NthGraph() : pathStroke(3.f, PathStrokeType::curved, PathStrokeType::butt)
{
}

NthGraph::~NthGraph()
{
}

void NthGraph::initializeNodes()
{
    //creates some nodes to start with, can't be done in the constructor
    //because getWidth and getHight won't work untill after construction
    
    nodes.push_back(Point<int>(0,getHeight()));
    nodes.push_back(Point<int>(getWidth() - getWidth()/1.5, getHeight() - getHeight()/6));
    nodes.push_back(Point<int>(getWidth() - getWidth()/2, getHeight() - getHeight()/1.5));
    nodes.push_back(Point<int>(getWidth() - getWidth()/6, getHeight() - getHeight()/2));
    nodes.push_back(Point<int>(getWidth(), 0));
}
void NthGraph::mouseDown(const MouseEvent& e)
{
    //inserts a new node
    
    //int to keep track of the node number being looked at
    int n = 0;
    
    //iterate through the nodes
    for(std::vector<Point<int>>::iterator it = nodes.begin(); it != nodes.end(); it++ )
    {
        //check to see if the node is in dragging distance
        if(e.getPosition().getDistanceFrom(*it) < 10)
        {
            //set the draggingNodeNum and exit the function
            draggingNodeNum = n;
            return;
        }
        //check to see if the position of the mouse is greater than the x positon of the node being looked at
        if(e.getPosition().x > it->x)
        {
            //add to the value of n to determin where the new node is to be inserted and assign it to the draggingNodeNum
            draggingNodeNum = ++n;
        }

    }
    
    //create an iterator and set it to the begining of the nodes vector
    std::vector<Point<int>>::iterator inserterIt;
    inserterIt = nodes.begin();
    
    //insert a new vector at position n
    nodes.insert(inserterIt+n, e.getPosition());

    //repaint the graph
    repaint();
    
}
void NthGraph::mouseDrag(const MouseEvent& e)
{

   //make a point
    Point<int> p1(e.getPosition());
        
    //check to make sure the point does not go to the left or right of it's neighbouring nodes or go outside the graph
    if(draggingNodeNum == 0)
    {
        p1.x = 0;
    }
    else if(draggingNodeNum == nodes.size() - 1)
    {
        p1.x = getWidth();
    }
    else if(p1.x > nodes[draggingNodeNum+1].x)
    {
        p1.x = nodes[draggingNodeNum+1].x - 1;
    }
    else if(p1.x < nodes[draggingNodeNum-1].x)
    {
        p1.x = nodes[draggingNodeNum-1].x + 1;
    }
    
    if(p1.y >= getHeight())
    {
        p1.y = getHeight();
    }
    else if(p1.y <= 0)
    {
        p1.y = 0;
    }
    
    //assing the node being dragged to the mouse position
    nodes[draggingNodeNum] = p1;
    
    //repaint the graph
    repaint();
}
void NthGraph::mouseDoubleClick(const MouseEvent& e)
{
    //erase a node
    
    //check to see if the the node is not the first or last node and the total number of nodes at least 4
    if(nodes.size() > 4 && draggingNodeNum != 0 && draggingNodeNum != nodes.size() - 1)
    {
        //erase the node and repaint
        nodes.erase(nodes.begin() + draggingNodeNum);
        repaint();
    }
    
}
void NthGraph::paint(Graphics& g)
{

    //one time update to give the nodes some positon on start
    if(update)
    {
        initializeNodes();
        update = false;
    }
    
    //draw the background of the graph white
    g.setColour (Colours::white);
    g.fillRect(getLocalBounds());   // draw an outline around the component
    
    
    //drawing dashed lines coneting the nodes
    //==============================================================================
    
    //an array that defines how many pixles each line segment and break will be
    float dashArray[4]{4,4,4,4};
    
    g.setColour (Colours::grey);
    
    //iterate though the nodes
    for(std::vector<Point<int>>::iterator it = nodes.begin(); it != nodes.end()-1; it++ )
    {
        std::vector<Point<int>>::iterator nextIt = it+1;
        //make a line between the current and next node
        Line<float> dashedLine(float(it->x), float(it->y), float(nextIt->x), float(nextIt->y));
        //draw the line
        g.drawDashedLine(dashedLine, dashArray, 4, 1.f, 0);
    }
    
    //draw our multipoint line
    //==============================================================================
    g.setColour (Colours::black);
    
    
    //runn our multipoint Bezier Curve drawing function
    multipointBezierCurve(nodes, path);
    
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
    
    //set the line at some x position greater than 0 and less than getWidth() then draw it all the wat to the top + 1
    //remember the top it 0 since the pixels count up from the top down
    line.setStart(float(getWidth()/2), float(getHeight()));
    line.setEnd(float(getWidth()/2), 1.f);
    
    //clip the line when it intersects with the closed path
    line = pathCopy.getClippedLine(line, true);
    
    //extract the y value at the entered x with line.getEndY()
    //note this will be upsidown since the pixles count up from the top of the graph down
    
    //DBG(line.getEndY());
    
    //==============================================================================
    
    //create a stroked path from the cubic path we made and fill it
    pathStroke.createStrokedPath(path, path);
    g.fillPath(path);
    
    //draw some Ellipses to represent the nodes
    for(std::vector<Point<int>>::iterator it = nodes.begin(); it != nodes.end(); it++ )
    {
        g.setColour (Colours::blue.interpolatedWith(Colours::red.interpolatedWith(Colours::green, float(it->y)/float(getHeight())), float(it->x)/float(getWidth())));
        g.fillEllipse(it->getX() - 5, it->getY() - 5, 10, 10);
    }

}

void NthGraph::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NthGraph::multipointBezierCurve(const std::vector<Point<int>> &cp, Path &path)
{
    //make some flotes to represent the mid point between two nodes
    float midx, midy;
    
    //make a couple of points 0 and 1
    Point<int> p0, p1;
    
    //clear the old path and reset it to the first node
    path.clear();
    path.startNewSubPath(cp[0].x, cp[0].y);
    
    //itereate through the nodes starting at the second node and ending one before the last node
    for(int i = 1; i < cp.size() - 2; i++)
    {
        //set point 1 to the current node and 0 to the next node
        p0 = cp[i];
        p1 = cp[i+1];
        
        //find the x and y position on the graph that is half way between the two nodes, the middle of the dashed line
        midx = float(p0.x + p1.x)/2.f;
        midy = float(p0.y + p1.y)/2.f;
        
        //draw a quadratic curve from the current positon (cp[i-1], where the last curve ended)
        //to the mid point just calculated and using the current node as the control point
        path.quadraticTo(float(p0.x), float(p0.y), midx, midy);
    }
    
    //for the last curve their is no middle point to draw to so just use the second to last node as our controle point
    p0 = cp[cp.size() - 2];
    p1 = cp[cp.size() - 1];
    path.quadraticTo(float(p0.x), float(p0.y), float(p1.x), float(p1.y));
    
}
