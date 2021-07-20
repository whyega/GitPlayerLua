#include "DrawHook.h"
#include <d3d9.h>

DrawHook::DrawHook() {
	hook_lost.onBefore += std::tuple{ this, &DrawHook::lost };
	hook_lost.install();
	hook_reset.onBefore += std::tuple{ this, &DrawHook::reset };
	hook_reset.install();
}

IDirect3DDevice9 *DrawHook::d3d9_device() {
	return *reinterpret_cast<IDirect3DDevice9 **>( 0xC97C28 );
}

void DrawHook::lost() {
	onPreReset();
}

void DrawHook::reset() {
	onPostReset();
}
