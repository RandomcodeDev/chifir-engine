Don't expect this list to be up to date or anything

* work on `Base`:
	- finish windows mutex implementation
	- other sync objects (semaphore, fence, event, etc)?
	- properly make everything thread safe
	- look into ways to better design things for multithreading, right now things are pretty single threaded
	- uwp support is coming along, also needs to be in `videosystem`
    	- need to fix code to get the AppData path
    	- need to do callbacks/event handling
    	- needs multithreading for event handling
* Switch stuff
	- need to fix vulkan crash, low priority
* add cfgvars
	- probably in `Base`
* RHI
	- dx12 is up to swap chain creation, vulkan is nearly there
* add a renderer
* add input
* wire up a ui library to directly use the engine's stuff instead of os/api-specific stuff
* add physics (`PhysX` or custom?)
* make an ecs or integrate flecs
* levels (probably just serialized entities)
* serialization
* network
