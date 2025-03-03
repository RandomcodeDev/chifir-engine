= Renderer architecture
The renderer will be implemented in multiple layers, flexible enough to support drawing and post-processing fairly complex
scenes, extensible with more techniques and passes, and simple to use for the rest of the engine.

== Hardware interface
The hardware interface is an abstraction of Vulkan/Direct3D/GNM/whatever other ungodly API I have to deal with. It's low
level, and implements render targets, materials, and geometry primitives, as well as special render targets just for going
to the screen (they wrap the swap chain images).
#list(
	[Handles `VkInstance`/`IDXGIFactory`, `VkDevice`/`ID3D12Device`, `VkCommandBuffer`/`ID3D12GraphicsCommandList`, `VkSwapChainKHR`/`IDXGISwapChain`],
	[Creates and manages geometry (VB+IB), textures, render targets, shaders, materials (texture + shader)],
	[Handles drawing given geometry + material],
)

== Rendering pipeline
Handles the process of taking data (model, position, etc of objects in scene, and general properties of the world) and
using the hardware interface to render and post-process all of it.
#list(
	[Calls for drawing objects and adding lights],
	[Uses multiple render passes to light and post-process the scene],
	[Rasterization-based lighting passes],
	[Ray-tracing-based passes],
	[Common post-processing passes],
)

== Render system
Calls into the rendering pipeline to draw scenes from different cameras, such as the player's eyes/over the shoulder, cinematic cameras,
mirrors and other reflective surfaces, and literal cameras.
#list(
	[ECS system that iterates over objects in the scene],
	[Sets parameters based on scene, such as sky details (even that could be an entity)],
)

