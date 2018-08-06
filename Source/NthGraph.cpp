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
    startTimer(40);
}

NthGraph::~NthGraph()
{
}

void NthGraph::initializeNodes()
{
    //creates some nodes to start with, can't be done in the constructor
    //because getWidth and getHight won't work untill after construction
    
    nodes.push_back(Point<int>(0,getHeight()));
    quadPaths.add(new Path());
    nodes.push_back(Point<int>(getWidth() - getWidth()/1.5, getHeight() - getHeight()/6));
    quadPaths.add(new Path());
    nodes.push_back(Point<int>(getWidth() - getWidth()/2, getHeight() - getHeight()/1.5));
    quadPaths.add(new Path());
    nodes.push_back(Point<int>(getWidth() - getWidth()/8, getHeight() - getHeight()/2));
    quadPaths.add(new Path());
    nodes.push_back(Point<int>(getWidth(), 0));
    //quadPaths.add(new Path());
    
    multipointBezierCurve();
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
    
    //insert a new node/path at position n
    nodes.insert(inserterIt+n, e.getPosition());
    
    //if(n == 1)
    quadPaths.insert(n, new Path());

    runPathing = true;
    //repaint the graph
    repaint();
    
}
void NthGraph::mouseDrag(const MouseEvent& e)
{
    //dragg a node
    
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
    
    if(frame)
    {
        runPathing = true;
        
        //repaint the graph
        repaint();
        frame = false;
    }
}
void NthGraph::timerCallback()
{
    frame = true;
}
void NthGraph::mouseDoubleClick(const MouseEvent& e)
{
    //erase a node
    
    //check to see if the the node is not the first or last node and the total number of nodes at least 4
    if(nodes.size() > 4 && draggingNodeNum != 0 && draggingNodeNum != nodes.size() - 1)
    {
        //erase the node/path and repaint
        nodes.erase(nodes.begin() + draggingNodeNum);
//        if(draggingNodeNum == 1)
//        {
//            quadPaths[0]->clear();
//        }
        quadPaths.remove(draggingNodeNum);
        
        runPathing = true;
        repaint();
    }
    
}
void NthGraph::paint(Graphics& g)
{
    

    //one time update to give the nodes some positon on start
    if(update)
    {
        initializeNodes();
        //update = false;
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

    //one run update to set up
    if(update)
    {
        quadPaths.ensureStorageAllocated(100);
        update = false;
    }
    
    //runn our multipoint Bezier Curve drawing function
    if(runPathing)
    {
        multipointBezierCurve();
        runPathing = false;
    }
    
    //make a copy of our path then draw it
    for(int i = 1; i < quadPaths.size(); i++)
    {
        //change the colour for one path for testing purposes
        if(i == 2 && quadPaths.size() > 3)
        {
             g.setColour (Colours::red);
        }
        else
        {
             g.setColour (Colours::grey);
        }
        
        Path pathCopy(*quadPaths[i]);
        
        pathCopy.lineTo(quadPaths[i]->getBounds().getRight(), 0);
        pathCopy.lineTo(quadPaths[i]->getBounds().getX(), 0);
        pathCopy.closeSubPath();
        
        path.addPath(pathCopy);
        
        g.strokePath(*quadPaths[i], pathStroke);
        
        
    }

    
    //take a little break to extract data
    //==============================================================================
    
    //make a line to measure against an axis
    Line<float> line;

    //set the line at some x position greater than 0 and less than getWidth() then draw it all the wat to the top + 1
    //remember the top it 0 since the pixels count up from the top down
    line.setStart(float(getWidth()/2), float(getHeight()));
    line.setEnd(float(getWidth()/2), 1.f);
    
    //clip the line when it intersects with the closed path
    line = path.getClippedLine(line, true);
    
    //draw the line for visual purposes (optional)
    g.setColour (Colours::blue);
    g.drawLine(line);
    
    //extract the y value at the entered x with line.getEndY()
    //note this will be upsidown since the pixles count up from the top of the graph down (DBG is just an example, **disabled)
    
    //DBG(line.getEndY());
    
    //==============================================================================
    
    //clear the path copy
    path.clear();
    
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

void NthGraph::multipointBezierCurve()
{

    //This method updates the Bezeir curve for efficiency purposes only the curves that are changing are calculated
    
    //the start and end of most curves is the midpoint between 2 controle nodes
    float midx, midy;
    int iBump;
    
    //this represents the path being calculated
    int i;
    
    //the index at which the paths are no longer needed to be calculated
    int end;
    
    //true if the final path must be calculated
    bool runLastNode;
    
    bool removepath0 = false;
    
    
    //there are several situations in which the calculation requires different starting conditions
    if((nodes.size() == 6 && (draggingNodeNum == 3 || draggingNodeNum == 2 ))|| (nodes.size() == 7 && draggingNodeNum == 3) || (nodes.size() == 5 || nodes.size() == 4))
    {
        //this first one is for when to entire curve needs to be calcuated
        //quadPaths[0]->clear();
        i = 1;
        iBump = 0;
        midy = nodes[0].y;
        midx = nodes[0].x;
        end = int(quadPaths.size()) - 2;
        runLastNode = true;

    }
    else if(draggingNodeNum > 2 && draggingNodeNum < int(nodes.size()) - 3)
    {
        //if neither the first or last three paths need to be calculated
        i = draggingNodeNum - 1;
        iBump = 0;
        midx = float(nodes[i-1].x + nodes[i].x)/2.f;
        midy = float(nodes[i-1].y + nodes[i].y)/2.f;
        end = draggingNodeNum + 1;
        runLastNode = false;
        
    }
    else if(draggingNodeNum <= 2)
    {
        //if any of the first three nodes need to be calculated
        //quadPaths[0]->clear();
        removepath0 = true;
        i = 1;
        iBump = 0;
        midx = nodes[0].x;
        midy = nodes[0].y;
        end = draggingNodeNum + 1;
        runLastNode = false;
    }
    else if(draggingNodeNum >= int(nodes.size()) - 3)
    {
        //if any of the last three nodes need to be calculated
        i = draggingNodeNum - 1;
        iBump = 0;
        midx = float(nodes[i-1].x + nodes[i].x)/2.f;
        midy = float(nodes[i-1].y + nodes[i].y)/2.f;
        end = int(quadPaths.size()) - 2;
        runLastNode = true;
    }
    else
    {
        //this is just for safty, should never run
        i = 1;
        iBump = 0;
        midy = nodes[0].y;
        midx = nodes[0].x;
        end = int(quadPaths.size()) - 2;
        runLastNode = true;
    }
    
    
    //itterate through the paths and perform the quadic calculation
    for(; i <= end; i++)
    {
        quadPaths[i]->clear();
        quadPaths[i]->startNewSubPath(midx, midy);
        
        midx = float(nodes[i+iBump].x + nodes[i+iBump+1].x)/2.f;
        midy = float(nodes[i+iBump].y + nodes[i+iBump+1].y)/2.f;
        
        quadPaths[i]->quadraticTo(float(nodes[i+iBump].x), float(nodes[i+iBump].y), midx, midy);
    }
    
    //special calculation if the last path needs to be calculated
    if(runLastNode)
    {
        quadPaths[i]->clear();
        quadPaths[i]->startNewSubPath(midx,midy);
        quadPaths[i]->quadraticTo(float(nodes[nodes.size() - 2].x), float(nodes[nodes.size() - 2].y), float(nodes[nodes.size()-1].x), float(nodes[nodes.size()-1].y));
    }
}
