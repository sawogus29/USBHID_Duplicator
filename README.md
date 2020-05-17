1. HID 라이브러리는 MyHID라는 User라이브러리로 만들었습니다. MyHID 폴더를 아두이노  User Libarary 폴더에 넣어주세요
-> #include <HID.h>대신 #include <MyHID.h> 쓰시면 됩니다.

2. USB HOST shield 라이브러리에서 약간 코드를 수정했습니다.
 USB host shield\usbhid.cpp 파일을 깃헙에 올려놓은 파일로 교체해주세요