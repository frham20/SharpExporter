#ifndef SHARPEXPORTER_H
#define SHARPEXPORTER_H

#include <Windows.h>
#include <vector>

typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;
typedef signed int     int32;
typedef signed short   int16;
typedef signed char    int8;

#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnPlugin.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MColorArray.h>
#include <maya/MPxFileTranslator.h>

#endif