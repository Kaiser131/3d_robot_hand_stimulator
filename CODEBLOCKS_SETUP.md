# Code::Blocks Quick Run (Windows + MinGW)

This repo includes a ready Code::Blocks project:
- `RobotArmSimulation.workspace`
- `RobotArmSimulation.cbp`

## 1) Open and build
1. Open Code::Blocks.
2. `File -> Open...` and open `RobotArmSimulation.workspace`.
3. Select the **Debug** target.
4. `Build -> Build` then `Build -> Run`.

## 2) If you get missing headers (GL/glut.h)
You need **FreeGLUT** installed and visible to the compiler.

## 2.5) If you saw tons of `<cmath>` errors
That was caused by a Windows case-insensitive include collision where a project file named `Math.h` could be picked up when the standard library tries to include `<math.h>`. This repo uses `MathUtil.h` to avoid that.

Common easiest options:
- Install MSYS2 and then: `pacman -S mingw-w64-x86_64-freeglut` (then use that MinGW toolchain), or
- Use a Code::Blocks distribution that already ships with FreeGLUT.

If you installed FreeGLUT manually, set these in Code::Blocks:
- `Settings -> Compiler... -> Search directories -> Compiler`
  - add FreeGLUT `include/`
- `Settings -> Compiler... -> Search directories -> Linker`
  - add FreeGLUT `lib/`

## 3) If build succeeds but running fails (freeglut.dll not found)
Copy `freeglut.dll` next to the generated exe:
- `bin/Debug/RobotArmSimulation.exe` (Debug)
- `bin/Release/RobotArmSimulation.exe` (Release)

OR add the folder containing `freeglut.dll` to your `PATH`.

## 5) Linker error: cannot find -lfreeglut / -lglut32
Different Windows setups use different GLUT library names:

- Many Code::Blocks + MinGW bundles ship **GLUT** as `glut32` (`libglut32.a`), so this project links with `-lglut32` by default.
- If you installed **FreeGLUT** (common via MSYS2), the library name is often `freeglut` (`libfreeglut.a`).

If you see a linker error like `cannot find -lglut32` or `cannot find -lfreeglut`:
1. Check which file exists under your MinGW lib folder, e.g.
  - `C:\Program Files (x86)\CodeBlocks\MinGW\lib\libglut32.a`
  - OR `...\lib\libfreeglut.a`
2. In Code::Blocks open:
  - `Project -> Build options... -> Linker settings`
3. Replace the missing one with the one you have:
  - Use `glut32` if you have `libglut32.a`
  - Use `freeglut` if you have `libfreeglut.a`
4. If neither exists, install FreeGLUT and add its `lib/` folder to:
  - `Settings -> Compiler... -> Search directories -> Linker`

## 4) Controls
See `README.md`.
