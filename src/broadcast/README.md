ffmpeg complile 
apt-get install libssl-dev -y


apt install libsdl2-dev libsdl2-2.0-0 -y
apt install libfdk-aac-dev -y
git clone git@github.com:FFmpeg/FFmpeg.git  ffmpeg
git checkout release/3.3

./configure --disable-yasm --enable-shared  --enable-ffplay--enable-debug=3  --disable-optimizations --disable-mmx --disable-stripping

do it 
 ./configure --disable-yasm --enable-shared  --enable-ffplay --enable-debug=3  --disable-optimizations --disable-mmx --disable-stripping --enable-gpl --enable-nonfree --enable-libfdk-aac



./configure --disable-yasm --enable-shared  --enable-ffplay --enable-debug=3  --disable-optimizations --disable-mmx --disable-stripping --enable-gpl --enable-nonfree --enable-libfdk-aac  --enable-libmp3lame   --enable-nonfree --enable-libx264


apt-get install -y libx264-dev

wget https://downloads.sourceforge.net/lame/lame-3.100.tar.gz && \
tar xvf lame-3.100.tar.gz && cd lame-3.100 && \
PATH="$HOME/bin:$PATH" \
./configure \
       --enable-shared \

make install



make -j8

make install



To debug webrtc


chrome://webrtc-internals

firefox
about:webrtc
about:config

enable logging of webrtc

chrome --enablewebrtc log


To compile webrtc
Download and Install the Chromium depot tools.

https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/

*Linux (Ubuntu/Debian)*
A script is provided for Ubuntu, which is unfortunately only available after your first gclient sync:

./build/install-build-deps.sh


https://webrtc.github.io/webrtc-org/native-code/development/

export PATH=/export/webrtc/depot_tools:$PATH

- mkdir webrtc-checkout
- cd webrtc-checkout
- fetch --nohooks webrtc
- gclient sync

 Note: Remove ffmpeg internal build from webrtc with rtc_use_h264=false

- $ cd src
- $ git checkout -b m84 refs/remotes/branch-heads/4147
- $ gclient sync
- In OSX 10.14.16 this works:
- $ gn gen out/m84 --args='is_debug=false is_component_build=false is_clang=true rtc_include_tests=false rtc_use_h264=false rtc_enable_protobuf=false use_rtti=true mac_deployment_target="10.11" use_custom_libcxx=false'
- In Linux Debian Stretch with GCC 6.3 this works:
- $ gn gen out/m84 --args='is_debug=true symbol_level=2 is_component_build=false is_clang=false rtc_include_tests=false rtc_use_h264=false rtc_enable_protobuf=false use_rtti=true use_custom_libcxx=false treat_warnings_as_errors=false use_ozone=true'

- Then build it:
- $ ninja -C out/m84


for release

gn gen out/m84_release --args='is_debug=false symbol_level=0 is_component_build=false is_clang=false rtc_include_tests=false rtc_use_h264=false rtc_enable_protobuf=false use_rtti=true use_custom_libcxx=false treat_warnings_as_errors=false use_ozone=true'

ninja -C out/m84_release


-Then build mediaserver/src/webrtc:



$ cmake . -Bbuild \
  -DLIBWEBRTC_INCLUDE_PATH:PATH=/home/foo/src/webrtc-checkout/src \
  -DLIBWEBRTC_BINARY_PATH:PATH=/home/foo/src/webrtc-checkout/src/out/m84/obj

$ make -C build/
**********************************************************************************************************************************************************
For windows
1. Install git:

if you haven???t installed a copy of git, open https://git-for-windows.github.io/, download and install, suggest to select ???Use Git from Windows Command Prompt??? during installation.

2. Fetch depot_tools:

follow https://sourcey.com/articles/building-and-installing-webrtc-on-windows use 7z to extract to c:\webrtc\depot_tools

set system path 

open git cmd not git basah

mkdir webrtc-checkout
cd webrtc-checkout
fetch --nohooks webrtc

do not forget to set for using visual studio for builing code
set DEPOT_TOOLS_WIN_TOOLCHAIN=0

cd src
git branch -r
git checkout branch-heads/m75

gn gen out/x64/Debug --args="is_debug=true use_rtti=true target_cpu=\"x64\""
ninja -C out/x64/Debug boringssl field_trial_default protobuf_full p2p

**********************************************************************************************************************************************************
For android

export PATH=/workspace/depot_tools:$PATH

mkdir webrtc_android/

cd webrtc_android

fetch --nohooks webrtc_android.
cd src git checkout branch-heads/m75
Then type gclient sync

git remote add arvind git@github.com:akumrao/webrtc-android.git

git remote update

gclient sync -D


git checkout 

      1 * (HEAD detached at arvind/multiplex-video)
      
      2   master


cd src

 ./build/install-build-deps.sh
 
 ./build/install-build-deps-android.sh



/*Generate compilation script*/

src/

gn gen out/arm --args='target_os="android" target_cpu="arm"'

ninja -C out/arm/

cp  ./out/arm/clang_x64/protoc ./out

delete our rm -rf out/arm


open android studio

open project /workspace/android_arwebrtc/MixedRealityWebRTCUnityDemo

open terminal in android studio

./gradlew clean

./gradlew genWebrtcSrc

./gradlew build

replace binaries generated by unity 

run install app

give permission to camera and mic at android setttings

open debug configuration 

select dual mode debug


Pplay pcm
ffplay -autoexit -f s16le -ar 48000 -ac 2 /var/tmp/out.pcm


ffmpeg -f s16le -ar 48000 -ac 2 -i  /var/tmp/test.mp3 file.wav


for aac for opus rencoding use 48000
ffmpeg -i /var/tmp/test.mp3 -ar 48000 -ac 2 -f s16le out.pcm


for aac and mp3 re encoding use 44100
ffmpeg -i /var/tmp/test.mp3 -ar 44100 -ac 2 -f s16le out.pcm



-acodec pcm_s16be: Output pcm format, signed 16 encoding, endian is big end (small end is le);
-ar 16000: The sampling rate is 16000
-ac 1: the number of channels is 1


**************************************************************************************


ffmpeg -re -i test.mp4 -g 52 -c:a aac -b:a 64k -c:v libx264 -b:v 448k -f mp4 -movflags frag_keyframe+empty_moov output.mp4



ffmpeg -i test.264  -i test.aac -f mp4 -movflags empty_moov+omit_tfhd_offset+frag_keyframe+default_base_moof /tmp/output1.mp4

ffmpeg -i test.264  -i test.aac -f mp4 -movflags empty_moov+omit_tfhd_offset+separate_moof+frag_custom /tmp/output2.mp4


ffmpeg -i kunal720.264  -i kunal720_track2.aac -f mp4 -movflags empty_moov+omit_tfhd_offset+frag_keyframe+default_base_moof /tmp/output1.mp4