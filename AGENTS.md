# CrimsonEngine Agent Guide

CrimsonEngine is a custom Godot fork for fully featured **3D-only** game development, with a strong bias toward first-person games. Treat this repository as an engine product, not a generic upstream Godot checkout.

## Project Scope

- CrimsonEngine keeps Godot's editor, import pipeline, scripting, GUI/HUD support, 3D rendering, 3D physics, 3D navigation, animation, audio, packaging, and extensibility.
- CrimsonEngine removes or hides 2D game creation workflows, 2D gameplay nodes/resources, 2D physics, and 2D navigation.
- Control/UI and canvas internals are still allowed when they are needed for editor UI, game menus, HUDs, overlays, inspectors, and tooling. 3D-only does not mean "no GUI".
- First-person game workflows are a priority: 3D scene creation, camera/player controllers, 3D input, character movement, collision, navigation, animation, audio spaces, environment setup, import flow, and runtime/editor performance.

## Current Fork Policy

- `disable_2d` defaults to `True` in `SConstruct`.
- `disable_asset_store` defaults to `True` in `SConstruct`.
- `_2D_DISABLED`, `PHYSICS_2D_DISABLED`, and `NAVIGATION_2D_DISABLED` are expected in the default editor build.
- `ASSET_STORE_DISABLED` is expected in the default editor build.
- The Asset Store should not be registered as an editor main screen or Project Manager tab.
- Plugin management must remain available from the top menu via `Project -> Plugins...`.
- The empty-scene workflow should not offer `2D Scene`.
- `ClassDB` should not expose user-addable 2D gameplay classes such as `Node2D`, `Sprite2D`, `PhysicsServer2D`, or `NavigationServer2D`.

## Main Edited Areas

- Build options and global defines live in `SConstruct`.
- Editor build folder inclusion lives in `editor/SCsub`.
- 2D scene/resource/server build traversal is guarded in:
  - `scene/SCsub`
  - `scene/resources/SCsub`
  - `servers/SCsub`
  - `editor/scene/SCsub`
- Runtime/editor class registration is guarded in:
  - `scene/register_scene_types.cpp`
  - `editor/register_editor_types.cpp`
- User-facing scene creation affordances are guarded in:
  - `editor/docks/scene_tree_dock.cpp`
  - `editor/scene/scene_create_dialog.cpp`
- Asset Store and plugin menu behavior is handled in:
  - `editor/editor_node.cpp`
  - `editor/editor_node.h`
  - `editor/project_manager/project_manager.cpp`
  - `editor/settings/editor_settings.cpp`
  - `editor/settings/editor_feature_profile.*`

## Do

- Preserve the 3D-only identity. When adding editor or runtime features, ask whether they support 3D/first-person workflows.
- Keep UI, Control nodes, theme code, and CanvasItem internals when required by the editor or by HUD/menu workflows.
- Use compile-time guards for optional/removed subsystems, and make sure both includes and use sites are guarded.
- Deregister user-facing classes in `ClassDB`, remove editor plugins, and remove visible creation paths together. A class being unregistered is not enough if a button still creates it directly.
- Search broadly for user-facing strings when removing a workflow, e.g. `2D Scene`, `Asset Store`, `Node2D`, `PhysicsServer2D`, `NavigationServer2D`.
- Prefer small, targeted compile checks while iterating.
- Use `rg`/`rg --files` for searches.
- Put temporary logs/scripts under `C:\tmp`.

## Do Not

- Do not re-enable 2D gameplay nodes, 2D physics, 2D navigation, TileMap workflows, or 2D editor plugins without explicit approval.
- Do not remove GUI/HUD/editor UI infrastructure just because names contain `2D`, `Canvas`, `CanvasItem`, `Texture2D`, `Rect2`, `Vector2`, or `Transform2D`. Some of these are core/editor primitives.
- Do not restore the Asset Store tab, Asset Store shortcut, or Project Manager Asset Store view unless explicitly asked.
- Do not hide or remove the plugin management page. It is intentionally available through `Project -> Plugins...`.
- Do not run a full rebuild after every tiny `.cpp` edit.
- Do not revert unrelated work in the tree.

## Build Workflow

For quick iteration, compile the touched object first:

```powershell
scons platform=windows target=editor bin\obj\editor\project_manager\project_manager.windows.editor.x86_64.obj -j23
```

Use the matching object path for the file being changed. After the focused object checks pass, relink the editor executable:

```powershell
scons platform=windows target=editor bin\godot.windows.editor.x86_64.exe bin\godot.windows.editor.x86_64.console.exe -j23
```

Expect a much larger rebuild when changing `SConstruct`, global compiler defines, generated headers, build traversal, or shared headers. That is normal because SCons invalidates objects whose signatures may have changed.

## Suggested Verification

- Confirm the editor executable timestamp changed:

```powershell
Get-Item bin\godot.windows.editor.x86_64.exe | Select-Object Name,LastWriteTime,Length
```

- For 2D deregistration checks, run a headless script that queries `ClassDB` for classes such as `Node2D`, `Sprite2D`, `PhysicsServer2D`, and `NavigationServer2D`.
- Open the editor and check that the empty scene prompt starts with `3D Scene`, `User Interface`, and `Other Node`, with no `2D Scene`.
- Check that the Asset Store main screen/tab is gone and `Project -> Plugins...` still opens the project settings plugin page.

