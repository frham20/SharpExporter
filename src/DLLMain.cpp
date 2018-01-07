#include "SharpExporter.h"
#include "EnvRenderExporter.h"
#include "ModelExporter.h"

_declspec(dllexport) 
MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);
	status = plugin.registerFileTranslator("Sharp EnvRender Exporter", "", Sharp::EnvRenderExporter::Create);
	return status;
}

_declspec(dllexport) 
MStatus uninitializePlugin(MObject obj) 
{
	MStatus   status;
	MFnPlugin plugin( obj );

	//unregister our export plugin
	status =  plugin.deregisterFileTranslator("Sharp EnvRender Exporter");
	if (!status) 
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	return status;
}
