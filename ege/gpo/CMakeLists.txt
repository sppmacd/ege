set(SOURCES
	"GameplayObject.h"
	"GameplayObjectManager.cpp"
	"GameplayObjectManager.h"
	"GameplayObjectRegistry.h"
	"GameplayObjectRegistry.inl"
)

ege_add_module(gpo)
ege_depend_module(gpo util)
