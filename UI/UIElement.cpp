#include "UIElement.h"
#include "Components/Geometry.h"
#include "Graphics/Program.h"
#include "Util/ServiceLocator.h"
#include "Graphics/Texture.h"

const GeometryComponent* Element::sUnitQuad = GeometryComponent::getUnitQuad();
const Program* Element::sScreenDraw = Program::getScreenDraw();

Element::Element(Texture* texture) {

}

void Element::draw() {
	sScreenDraw->use();
	// Send uniforms of some sort
	sUnitQuad->render();
}

RootElement::RootElement(std::string filename) :Element(NULL) {
	FileService file(filename);
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
			file.extract("`S`L", NULL);
		}
	}
	else {
		throw std::exception(filename.data());
	}
}

void RootElement::layout(int left, int right, int top, int bottom) {
	mBounds = Bounds2D{ top,bottom,left,right, 0 };
	for (auto e : mChildElements) {
		// Some type of calculation to get the correct bounding box for each subelement
		e->layout(left, right, top, bottom);
	}
}
