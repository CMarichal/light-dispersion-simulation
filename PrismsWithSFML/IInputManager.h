#ifndef IINPUT_MANAGER_H
#define IINPUT_MANAGER_H

// Interface for managers that handles user's inputs
class IInputManager
{
public:
	// Keys handled by the manager
	enum class Key
	{
		LEFT_ARROW,
		RIGHT_ARROW,
		UP_ARROW,
		DOWN_ARROW,
		Q,
		W,
		E,
		A,
		S,
		D,
		R,
		F,
		T,
		Y,
		G,
		H
	};

	virtual bool isKeyPressed(Key key) = 0;
	
};
#endif