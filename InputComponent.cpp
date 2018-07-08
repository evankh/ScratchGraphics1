#include "InputComponent.h"

void KeyboardInputComponent::handle(Event event) {
	mKeyboardEvents.push(event);
}
