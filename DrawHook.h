#ifndef DRAWHOOK_H
#define DRAWHOOK_H

#include <SRSignal/SRSignal.hpp>
#include <llmo/SRHookFast.h>
#include <type_traits>

class DrawHook {
	SRHook::Fast::Hook hook_lost{0x7F8152, 5};
	SRHook::Fast::Hook hook_reset{0x4CC979, 5};

public:
	DrawHook();

	// base events
	SRSignal<> onPreReset;
	SRSignal<> onPostReset;

	// d3d9 methods
	static class IDirect3DDevice9 *d3d9_device();

protected:
	void lost();
	void reset();
};

#endif // DRAWHOOK_H
