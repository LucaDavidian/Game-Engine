#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include "GUI.h"

class GUIPanel;
class GUIButton;
class GUILabel;
class GUITextField;

class GUIRenderer
{
private:
	class RenderGUIVisitor : public GUIVisitor
	{
	public:
		void Visit(GUIPanel*) override;
		void Visit(GUIButton*) override;   
		void Visit(GUILabel*) override;
		void Visit(GUITextField*) override;
	};
public:
	void Render();  	
};

#endif  // GUIRENDERER_H