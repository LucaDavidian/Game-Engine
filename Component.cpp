#include "Component.h"

unsigned int GetUniqueID()
{
	static unsigned id = 0;

	return id++;
}