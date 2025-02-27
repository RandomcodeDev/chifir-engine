= Engine components
The engine will be made of these pieces:
#table(
  columns: 4,
  [*Component*], [*Components needed*], [*Functionality*], [*Available in tools builds*],
  [`Base`], [system calls, possibly `libc`], [containers, basic algorithms, strings, data manipulation and serialization, Unicode handling,
   startup, shutdown, threading, synchronization, screen output, system information, basic file system functions,
   input, debugging features, logging], [yes],
  [`VideoSystem`], [`Base`], [abstracts a window or game console screen], [yes],
  [`Rhi*`], [`Base`, `VideoSystem`], [abstracts Vulkan, DirectX 12, GNM, OpenGL, DirectX 8/9, whatever else], [maybe?],
  [`Rhi`], [`Base`, `Rhi*`, `VideoSystem`], [exposes RHI backends], [maybe?],
  [`Math`], [none], [handles higher level math related things than `rtm`], [yes],
  [`Utility`], [`Base`], [localisation, thread pools (maybe), configuration], [yes],
  [`Texture`], [`Base`], [texture format], [yes],
  [`Mesh`], [`Base`], [mesh format], [yes],
  [`Pack`], [`Base`], [package file format], [yes],
  [`Launcher`], [`Base`, `Utility`], [loading an application DLL and the components it needs], [yes],
  [`Engine`], [`Base`, `Utility`], [cameras, scene management, entity component system, commonly used components (for entities), system management], [no],
  [`RenderSystem`], [`Base`, `Math`, `Rhi`, `VideoSystem`], [rendering scenes, UIs, anything else], [no],
  [`InputSystem`], [`Base`], [user input], [no],
  [`UiSystem`], [`Base`, `InputSystem`, `Math`, `RenderSystem`], [user interfaces], [no],
  [`PhysicsSystem`], [`Base`, `Math`], [simulates mechanical physics], [no],
  [`AnimationSystem`], [`Base`, `Math`], [controls skeletal animation], [no],
  [`AudioSystem`], [`Base`, `Math`], [handles audio], [no],
  [`Game`], [`AnimationSystem`, `Base`, `Engine`, `Utility`], [game functionality common between client and server, such as prediction and data parsing], [no],
  [`GameServer`], [`Base`, `Engine`, `PhysicsSystem`, `Utility`], [game functionality that happens on the server, such as simulation, player management, etc], [no],
  [`GameClient`], [`Base`, `Engine`, `InputSystem`, `RenderSystem`, `UiSystem`, `Utility`], [game functionality that happens on the client, such as rendering, player input, and possibly prediction], [no],
)
