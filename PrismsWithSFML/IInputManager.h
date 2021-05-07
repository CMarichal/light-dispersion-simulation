#pragma once

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
		Y
	};

	virtual bool isKeyPressed(Key key) = 0;
	
};