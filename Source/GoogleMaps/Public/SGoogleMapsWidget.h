// Copyright 2016, Sam Jeeves. All rights reserved.

#pragma once


#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class RUNRPG_API SGoogleMapsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGoogleMapsWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
