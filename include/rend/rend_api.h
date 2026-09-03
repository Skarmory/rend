#ifndef REND_REND_API_H
#define REND_REND_API_H

#pragma once

#if defined(_WIN32)

#if defined(REND_BUILD)
	#define REND_API __declspec(dllexport)
#else
	#define REND_API __declspec(dllimport)
#endif

#else

	#define REND_API

#endif

#endif

