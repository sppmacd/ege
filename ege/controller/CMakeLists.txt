set(SOURCES
	"Controllable.cpp"
	"Controllable.h"
	"Controller.cpp"
	"Controller.h"
	"ControlPacket.cpp"
	"ControlPacket.h"
)

ege_add_module(controller)
ege_depend_module(controller core) #?
#ege_depend_module(controller scene)
ege_depend_module(controller util)
