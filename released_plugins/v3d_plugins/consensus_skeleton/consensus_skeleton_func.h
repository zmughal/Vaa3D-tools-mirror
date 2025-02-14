/* consensus_skeleton_func.h
 * a plugin to merge multiple neurons by generating a consensus skeleton
 * 2012-05-02 : by Yinan Wan
 */
 
#ifndef __CONSENSUS_SKELETON_FUNC_H__
#define __CONSENSUS_SKELETON_FUNC_H__

#include <v3d_interface.h>

int consensus_skeleton_io(V3DPluginCallback2 &callback, QWidget *parent);
bool consensus_skeleton_io(const V3DPluginArgList & input, V3DPluginArgList & output);
void printHelp();

#endif

