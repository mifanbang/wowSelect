#ifndef __GPROGRESSBAR_H
#define __GPROGRESSBAR_H

#include "gui.h"



class GProgressBar : public GUI {
	GUI_CLASS("GProgressBar")

protected:
	virtual void initGUI();

public:
	// constructors
	GProgressBar(GUI *parent);
	~GProgressBar();

	UINT getMaximum();
	void setMaximum(UINT uMax);

	UINT getPosition();
	void setPosition(UINT uPos);

	void advance(int uDelta);
};


#endif  // __GPROGRESSBAR_H
