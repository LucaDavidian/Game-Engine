#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "Component.h"
#include "data structures/Vector.h"

#ifdef FAST_DELEGATES
	#include "fast delegates/connection.hpp"
#else
	#include "delegates/connection.hpp"
#endif  // FAST_DELEGATES

class PositionComponent;

class InputComponent : public Component
{
public:
	InputComponent(PositionComponent *positionComponent);
	~InputComponent() { for (ConnectionBase *connection : mConnections) { connection->Disconnect(); delete connection; } }

	void Init() override;

	void Connect(ConnectionBase *connection) { mConnections.InsertLast(connection); }

	virtual void OnKeyDown(int key);
	void OnMouseMove(int x, int y);
protected:
	PositionComponent *mPositionComponent;

	Vector<ConnectionBase*> mConnections;
};

#endif  // INPUT_COMPONENT_H

