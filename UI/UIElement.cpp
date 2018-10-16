#include "UIElement.h"
#include "../Geometry.h"
#include "../Program.h"
#include "../ServiceLocator.h"
#include "../Texture.h"

const Geometry* Element::sUnitQuad = Geometry::getUnitQuad();
const Program* Element::sScreenDraw = Program::getScreenDraw();

Element::Element(Texture* texture) {

}

void Element::draw() {
	sScreenDraw->use();
	// Send uniforms of some sort
	sUnitQuad->render();
}

RootElement::RootElement(std::string filename) :Element(NULL) {
	FileService& file = ServiceLocator::getFileService(filename);
	if (file.good()) {
		struct StackNode { Element* previous, *current; } workingStack{ NULL,this };
		while (file.good()) {
			// Add stuff to workingStack->current
			// If more indentation, or open brace, or whatever:
			//   workingStack = new StackNode{ workingStack, new WhateverElement() };
			// If less indentation, or close brace, or whatever:
			//   Element* child = workingStack.current;
			//   workingStack = workingStack.previous;
			//   if (!workingStack)	// We've somehow popped the root node
			//     LogError("Unexpected close of block, check that your braces are balanced!");
			//   else
			//     workingStack.current->addChild(child);	// Don't delete, that would be counterproductive
		}
	}
	else {
		ServiceLocator::getLoggingService().badFileError(filename);
	}
	file.close();
}

void RootElement::layout(int left, int right, int top, int bottom) {
	mBounds = Bounds{ top,bottom,left,right, 0 };
	for (auto e : mChildElements) {
		// Some type of calculation to get the correct bounding box for each subelement
		e->layout(left, right, top, bottom);
	}
}
