#pragma once

#include "dugl/common.h"


DUGL_NAMESPACE_BEGIN

/* Handle for a barrier user to wait for jobs to execute. Creation, destruction and waiting
   for jobs is handled through the AsyncDispatcher. */
class JobBarrier
{
public:
	virtual ~JobBarrier() = default;
};

DUGL_NAMESPACE_END
