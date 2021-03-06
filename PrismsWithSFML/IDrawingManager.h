#ifndef IDRAWING_MANAGER_H
#define IDRAWING_MANAGER_H

#include "stdafx.h"

// Interface for managers that draw the rendered scene on a support
class IDrawingManager
{
public:
	// handle the event when the windows is closed
	// return true when the window is closed
	virtual bool closedWindowEventHandler() =0;

	// clean the buffer and set a black background
	virtual void cleanWindow() =0;

	// display the results in the window
	virtual void display() =0;

	// draw a pixel (x,y) in a given GLM color
	virtual void drawPixel(int x, int y, const glm::vec3& color) =0;

	// save the image
	virtual void saveToFile(std::string filename) = 0;
};

#endif
