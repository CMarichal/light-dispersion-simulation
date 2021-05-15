#ifndef IINPUT_MANAGER_H
#define IINPUT_MANAGER_H

// Interface for input managers
class IInputManager
{
public:
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