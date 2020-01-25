#include "GUIComposite.h"

void GUIComposite::AddChild(GUIComponent *childComponent)
{
	childComponent->SetParent(this);

	childComponent->Update();

	mChildren.push_back(childComponent);
}

void GUIComposite::RemoveChild(GUIComponent *childComponent)
{
	std::vector<GUIComponent*>::const_iterator cit = mChildren.begin();

	while (cit != mChildren.end())
	{
		if (*cit == childComponent)
		{
			delete *cit;
			mChildren.erase(cit);
		}

		++cit;
	}
}

void GUIComposite::AcceptPreorder(GUIVisitor &visitor)
{
	for (GUIComponent *component : mChildren)
		component->AcceptPreorder(visitor);
}

void GUIComposite::AcceptPostorder(GUIVisitor &visitor)
{
	for (GUIComponent *component : mChildren)
		component->AcceptPreorder(visitor);
}