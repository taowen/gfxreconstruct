#!/usr/bin/env bash
# Owned by GFXReconstruct; invoked only by Wine's canonical component graph.
set -euo pipefail
[[ $# == 4 ]] || { echo 'Usage: build-bionic.sh NDK BIONIC_PREFIX BUILD OUTPUT' >&2; exit 2; }
repo="$(cd "$(dirname "$0")/.." && pwd)"
ndk="$(realpath "$1")"; prefix="$(realpath "$2")"
build="$(realpath -m "$3")"; output="$(realpath -m "$4")"
cmake -S "$repo/arlinux/bionic" -B "$build" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$ndk/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-29 -DANDROID_STL=c++_static \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DBIONIC_PREFIX="$prefix" \
    -DGFXRECON_ENABLE_OPENXR=OFF -DGFXRECON_ENABLE_USERFAULTFD=OFF
cmake --build "$build" --parallel "${JOBS:-4}" --target VkLayer_gfxreconstruct gfxrecon-convert gfxrecon-info
mkdir -p "$output/lib" "$output/bin"
cp "$build/android-layer/libVkLayer_gfxreconstruct.so" "$output/lib/"
cp "$build/convert/gfxrecon-convert" "$build/info/gfxrecon-info" "$output/bin/"
cp "$repo/LICENSE.txt" "$output/"
python3 - "$output" "$repo" <<'PACKAGE'
import hashlib,json,subprocess,sys
from pathlib import Path
out,repo=map(Path,sys.argv[1:])
manifest={'file_format_version':'1.2.0','layer':{
    'name':'VK_LAYER_LUNARG_gfxreconstruct','type':'GLOBAL',
    'library_path':'../lib/libVkLayer_gfxreconstruct.so','api_version':'1.4.0',
    'implementation_version':1,'description':'ARLinux Bionic Vulkan capture'}}
(out/'layer').mkdir()
(out/'layer/VkLayer_gfxreconstruct.json').write_text(json.dumps(manifest,indent=2)+'\n')
files={p.relative_to(out).as_posix():hashlib.sha256(p.read_bytes()).hexdigest()
       for p in out.rglob('*') if p.is_file()}
(out/'provenance.json').write_text(json.dumps(dict(format=1,abi='aarch64-bionic',
    repository='taowen/gfxreconstruct',commit=subprocess.check_output(
    ['git','-C',str(repo),'rev-parse','HEAD'],text=True).strip(),files=files),indent=2)+'\n')
PACKAGE
