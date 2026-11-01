#!/usr/bin/env python3
"""Build a signed development APK with the Android SDK/NDK and a JDK, without Gradle."""
import argparse
import os
import shutil
import subprocess
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--sdk', default=os.environ.get('ANDROID_SDK_ROOT') or os.environ.get('ANDROID_HOME'))
p.add_argument('--abi', default='arm64-v8a', choices=['arm64-v8a', 'x86_64'])
p.add_argument('--cmake', default=shutil.which('cmake'))
p.add_argument('--ninja', default=shutil.which('ninja'))
p.add_argument('--jobs', type=int, default=4)
p.add_argument('--keystore', type=Path, default=Path.home()/'.android/debug.keystore')
p.add_argument('--output', type=Path)
a = p.parse_args()
if not a.sdk or not a.cmake or not a.ninja:
    p.error('Provide --sdk, --cmake, and --ninja (or put CMake/Ninja on PATH).')
sdk = Path(a.sdk).resolve()
ndk = sdk/'ndk/23.2.8568313'
bt = sdk/'build-tools/36.0.0'
android = sdk/'platforms/android-36/android.jar'
for file in [ndk/'build/cmake/android.toolchain.cmake', bt/'aapt2', android]:
    if not file.exists(): p.error(f'Missing SDK/NDK component: {file}')
build = ROOT/'build-android'/a.abi
out = ROOT/'android-output'/a.abi
out.mkdir(parents=True, exist_ok=True)
a.output = (a.output or ROOT/f'android-output/Carrot-Fantasy-1.0.0-{a.abi}.apk').resolve()
a.output.parent.mkdir(parents=True, exist_ok=True)

def run(args):
    print('+', str(args[0]), ' '.join(str(x) for x in args[1:4]), flush=True)
    subprocess.run([str(x) for x in args], cwd=ROOT, check=True)

run([a.cmake, '-S', ROOT, '-B', build, '-G', 'Ninja', f'-DCMAKE_MAKE_PROGRAM={a.ninja}',
     f'-DCMAKE_TOOLCHAIN_FILE={ndk}/build/cmake/android.toolchain.cmake',
     f'-DANDROID_ABI={a.abi}', '-DANDROID_PLATFORM=android-23', '-DANDROID_STL=c++_static', '-DCMAKE_BUILD_TYPE=Release'])
run([a.cmake, '--build', build, '-j', a.jobs])
resources = out/'compiled-res.zip'
run([bt/'aapt2', 'compile', '--dir', ROOT/'proj.android/app/src/main/res', '-o', resources])
unsigned = out/'unsigned.apk'
app = ROOT/'proj.android/app/src/main'
generated = out/'generated'
generated.mkdir(exist_ok=True)
run([bt/'aapt2', 'link', '-o', unsigned, '-I', android, '--manifest', app/'AndroidManifest.xml',
     '--java', generated, '--min-sdk-version', '23', '--target-sdk-version', '36', resources])
engine = ROOT/'cocos2d/cocos/platform/android/java'
for aidl in (engine/'src').rglob('*.aidl'):
    run([bt/'aidl', '-I'+str(engine/'src'), '-p'+str(android.parent/'framework.aidl'), '-o'+str(generated), aidl])
classes = out/'classes'
classes.mkdir(exist_ok=True)
sources = list((engine/'src').rglob('*.java')) + list((app/'java').rglob('*.java')) + list(generated.rglob('*.java'))
jars = sorted((engine/'libs').glob('*.jar'))
classpath = os.pathsep.join(str(x) for x in [android]+jars)
# javac argument files avoid shell interpolation and command-length limits.
source_list = out/'java-sources.txt'
source_list.write_text('\n'.join('"'+str(x)+'"' for x in sources)+'\n')
run(['javac', '-encoding', 'UTF-8', '-source', '8', '-target', '8', '-bootclasspath', android,
     '-classpath', classpath, '-d', classes, '@'+str(source_list)])
class_jar = out/'classes.jar'
with zipfile.ZipFile(class_jar, 'w') as z:
    for file in classes.rglob('*.class'): z.write(file, file.relative_to(classes))
dex = out/'dex'
dex.mkdir(exist_ok=True)
run([bt/'d8', '--release', '--min-api', '23', '--lib', android, '--output', dex, class_jar]+jars)
lib = out/'libcarrotfantasy.so'
shutil.copy2(build/'libcarrotfantasy.so', lib)
run([ndk/'toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip', '--strip-unneeded', lib])
with zipfile.ZipFile(unsigned, 'a', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as z:
    for file in dex.glob('*.dex'): z.write(file, file.name)
    z.write(lib, f'lib/{a.abi}/libcarrotfantasy.so')
    for file in sorted((ROOT/'Resources').rglob('*')):
        if file.is_file() and file.suffix.lower() not in ['.zip', '.tps', '.txt']:
            z.write(file, 'assets/'+file.relative_to(ROOT/'Resources').as_posix())
aligned = out/'aligned.apk'
run([bt/'zipalign', '-P', '16', '-f', '4', unsigned, aligned])
if not a.keystore.exists():
    a.keystore.parent.mkdir(parents=True, exist_ok=True)
    run(['keytool', '-genkeypair', '-keystore', a.keystore, '-storepass', 'android', '-keypass', 'android',
         '-alias', 'androiddebugkey', '-keyalg', 'RSA', '-keysize', '2048', '-validity', '10000',
         '-dname', 'CN=Android Debug,O=Android,C=US'])
    a.keystore.chmod(0o600)
run([bt/'apksigner', 'sign', '--ks', a.keystore, '--ks-key-alias', 'androiddebugkey',
     '--ks-pass', 'pass:android', '--key-pass', 'pass:android', '--out', a.output, aligned])
run([bt/'apksigner', 'verify', '--verbose', a.output])
run([bt/'zipalign', '-c', '-P', '16', '4', a.output])
print('APK:', a.output, flush=True)
