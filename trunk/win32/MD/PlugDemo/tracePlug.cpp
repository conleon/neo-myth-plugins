#include "tracePlug.hpp"
#include "stdafx.h"

static bool silentMode = false;

const bool isSilentMode()
{
	return silentMode;
}

void forceSilentMode(const bool silent)
{
	silentMode = silent;
}

