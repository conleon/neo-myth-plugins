#ifndef __vgm2spc_snes_plug_interface__hpp__
#define  __vgm2spc_snes_plug_interface__hpp__

bool encodeVgm(const char* filename,char* outputBuffer,int* destLen);
bool encodeVgz(const char* filename,char* outputBuffer,int* destLen);
void encoderSetPluginDirectory(const std::string& plugDir);

#endif

