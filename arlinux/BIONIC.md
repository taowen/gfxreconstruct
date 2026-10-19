# Bionic Wine capture

Wine owns orchestration and distribution of this optional product. Use the sole
Android build entry and Wine's arlinux/BUILD.md; do not build from rootfs.
The native ARM64 Bionic layer captures Vulkan below Wine/DXVK/VKD3D, including
Wayland surface calls. It is disabled in ordinary application launches.
This recipe also builds Bionic info and JSON conversion tools. Replay uses a
matching GFXReconstruct replay build; successful conversion alone does not
prove faithful replay or correct application rendering.
