#ifndef GUICOMPOSITE_H
#define GUICOMPOSITE_H

#include "GUIComponent.h"
#include <vector>

class GUIComposite : public GUIComponent
{
public:
	~GUIComposite() { for (GUIComponent *component : mChildren) delete component; }

	void AddChild(GUIComponent *childComponent);
	void RemoveChild(GUIComponent *childComponent);

	void Update() override { GUIComponent::Update(); for (GUIComponent *component : mChildren) component->Update(); }

	void AcceptPreorder(GUIVisitor&) override;
	void AcceptPostorder(GUIVisitor&) override;

protected:
	GUIComposite() = default;
private:
	std::vector<GUIComponent*> mChildren;
};

#endif   // GUICOMPOSITE_H

