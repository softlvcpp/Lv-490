#include "Other/pch.h"
#include "Essentials/RuntimeFilter.h"

namespace log490
{

	void RuntimeLogFilter::setIndex(stream_t* stream)
	{
		if (stream)
		{
			filterIndex = 1;
			messageStream = stream;
		}
		else
		{
			filterIndex = 0;
			messageStream = nullptr;
		}
	}

	RuntimeLogFilter::RuntimeLogFilter()
	{
		messageStream = nullptr;
		filterIndex = 0;
	}

}


