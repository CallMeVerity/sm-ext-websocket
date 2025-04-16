#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "smsdk_ext.h"
#include <yyjson.h>
#include <IXWebSocket.h>
#include <IXWebSocketServer.h>
#include <IXHttpClient.h>
#include <yyjsonwrapper.h>
#include <task_context.h>
#include <ws_client.h>
#include <ws_server.h>
#include <http_request.h>
#include <queue.h>
#include <random>

class WebsocketExtension : public SDKExtension
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	virtual void SDK_OnUnload();
	virtual YYJsonWrapper *GetJSONPointer(IPluginContext *pContext, Handle_t handle);
	public:
		void AddTaskToQueue(ITaskContext *context);
};

class WsClientHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class WsServerHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class JSONHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class HttpHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

extern WebsocketExtension g_WebsocketExt;
extern HandleType_t g_htWsClient, g_htWsServer, g_htJSON, g_htHttp;
extern WsClientHandler g_WsClientHandler;
extern WsServerHandler g_WsServerHandler;
extern JSONHandler g_JSONHandler;
extern HttpHandler g_HttpHandler;
extern ThreadSafeQueue<ITaskContext *> g_TaskQueue;

extern const sp_nativeinfo_t ws_natives[];
extern const sp_nativeinfo_t ws_natives_server[];
extern const sp_nativeinfo_t json_natives[];
extern const sp_nativeinfo_t http_natives[];

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
