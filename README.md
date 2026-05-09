# Robot Arm Simulation (OpenGL + GLUT)

Interactive 3D robot arm simulation using classic OpenGL fixed-function pipeline (hierarchical transforms with `glPushMatrix`/`glPopMatrix`).

## Features
- 5 joints: base, shoulder, elbow, wrist, gripper open/close
- Joint angle limits + smooth motion
- Orbit camera (rotate + zoom)
- Basic lighting + depth testing
- Pick up and place an object when the gripper closes near it

## Build (Windows)

## Run in Code::Blocks (easy demo)
Open the workspace file and build/run:
- Open `RobotArmSimulation.workspace` in Code::Blocks
- Build target: **Debug**
- If you hit FreeGLUT setup issues (headers/DLL), follow `CODEBLOCKS_SETUP.md`

### Option A: vcpkg (recommended)
1. Install vcpkg and integrate with CMake.
2. Install FreeGLUT:
   - `vcpkg install freeglut`
3. Configure & build:
   - `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake`
   - `cmake --build build --config Release`

### Option B: System install
Install a GLUT implementation (e.g., FreeGLUT) and make sure CMake can find it (`GLUT_DIR` / `CMAKE_PREFIX_PATH`).

## Controls
Robot joints (hold keys for smooth movement):
- Base: `A` / `D`
- Shoulder: `W` / `S`
- Elbow: `Q` / `E`
- Wrist roll: `R` / `F`
- Gripper: `T` (open) / `G` (close)

Camera:
- Orbit: Arrow keys
- Orbit (mouse): Right mouse button drag
- Zoom: `+` / `-`
- Reset view: `C`

Misc:
- Reset arm pose: `X`
- Toggle instructions overlay: `I`
- Quit: `Esc`

## Notes
- Object pickup occurs automatically when the gripper is nearly closed and the gripper tip is close to the object.
- Releasing happens when you open the gripper.
