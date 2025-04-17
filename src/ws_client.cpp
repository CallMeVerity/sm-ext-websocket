#include "extension.h"

WebSocketClient::WebSocketClient(ix::WebSocket* existingSocket) : m_webSocket(existingSocket) {}

WebSocketClient::WebSocketClient(const char* url, uint8_t type) 
{
	m_webSocket = new ix::WebSocket();
	m_webSocket->setUrl(url);
	m_webSocket->disableAutomaticReconnection();
	m_callback_type = type;

	m_webSocket->setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
		switch (msg->type)
		{
			case ix::WebSocketMessageType::Message:
			{
				OnMessage(msg->str);
				break;
			}
			case ix::WebSocketMessageType::Open:
			{
				m_headers = msg->openInfo.headers;
				OnOpen(msg->openInfo);
				break;
			}
			case ix::WebSocketMessageType::Close:
			{
				OnClose(msg->closeInfo);
				break;
			}
			case ix::WebSocketMessageType::Error:
			{
				OnError(msg->errorInfo);
				break;
			}
		}
	});
}

WebSocketClient::~WebSocketClient() 
{
	if (!m_keepConnecting) m_webSocket->stop();

	if (pMessageForward) forwards->ReleaseForward(pMessageForward);
	if (pOpenForward) forwards->ReleaseForward(pOpenForward);
	if (pCloseForward) forwards->ReleaseForward(pCloseForward);
	if (pErrorForward) forwards->ReleaseForward(pErrorForward);
}

bool WebSocketClient::IsConnected()
{
	return m_webSocket->getReadyState() == ix::ReadyState::Open;
}

void WebSocketClient::OnMessage(const std::string& message) 
{
	if (!pMessageForward || !pMessageForward->GetFunctionCount())
	{
		return;
	}

	WsMessageTaskContext *context = new WsMessageTaskContext(this, message);
	g_WebsocketExt.AddTaskToQueue(context);
}

void WebSocketClient::OnOpen(ix::WebSocketOpenInfo openInfo) 
{
	if (!pOpenForward || !pOpenForward->GetFunctionCount())
	{
		return;
	}

	WsOpenTaskContext *context = new WsOpenTaskContext(this, openInfo);
	g_WebsocketExt.AddTaskToQueue(context);
}

void WebSocketClient::OnClose(ix::WebSocketCloseInfo closeInfo) 
{
	if (!pCloseForward || !pCloseForward->GetFunctionCount())
	{
		return;
	}
	
	WsCloseTaskContext *context = new WsCloseTaskContext(this, closeInfo);
	g_WebsocketExt.AddTaskToQueue(context);
}

void WebSocketClient::OnError(ix::WebSocketErrorInfo errorInfo) 
{
	if (!pErrorForward || !pErrorForward->GetFunctionCount())
	{
		return;
	}

	WsErrorTaskContext *context = new WsErrorTaskContext(this, errorInfo);
	g_WebsocketExt.AddTaskToQueue(context);
}

void WsMessageTaskContext::OnCompleted()
{
	const size_t messageLength = m_message.length() + 1;

	switch (m_client->m_callback_type)
	{
		case Websocket_STRING:
		{
			m_client->pMessageForward->PushCell(m_client->m_websocket_handle);
			m_client->pMessageForward->PushString(m_message.c_str());
			m_client->pMessageForward->PushCell(messageLength);
			m_client->pMessageForward->Execute(nullptr);
			break;
		}
		case WebSocket_JSON:
		{
			auto pYYJsonWrapper = CreateWrapper();

			yyjson_read_err readError;
			yyjson_doc *idoc = yyjson_read_opts(const_cast<char*>(m_message.c_str()), messageLength, 0, nullptr, &readError);

			if (readError.code)
			{
				yyjson_doc_free(idoc);
				smutils->LogError(myself, "parse JSON message error (%u): %s at position: %d", readError.code, readError.msg, readError.pos);
				return;
			}

			pYYJsonWrapper->m_pDocument = WrapImmutableDocument(idoc);
			pYYJsonWrapper->m_pVal = yyjson_doc_get_root(idoc);

			HandleError err;
			HandleSecurity pSec(nullptr, myself->GetIdentity());
			m_client->m_json_handle = handlesys->CreateHandleEx(g_htJSON, pYYJsonWrapper.release(), &pSec, nullptr, &err);

			if (!m_client->m_json_handle)
			{
				smutils->LogError(myself, "Could not create JSON handle (error %d)", err);
				return;
			}

			m_client->pMessageForward->PushCell(m_client->m_websocket_handle);
			m_client->pMessageForward->PushCell(m_client->m_json_handle);
			m_client->pMessageForward->PushCell(messageLength);
			m_client->pMessageForward->Execute(nullptr);

			handlesys->FreeHandle(m_client->m_json_handle, &pSec);
			break;
		}
	}
}

void WsOpenTaskContext::OnCompleted()
{
	m_client->pOpenForward->PushCell(m_client->m_websocket_handle);
	m_client->pOpenForward->Execute(nullptr);
}

void WsCloseTaskContext::OnCompleted()
{
	m_client->pCloseForward->PushCell(m_client->m_websocket_handle);
	m_client->pCloseForward->PushCell(m_closeInfo.code);
	m_client->pCloseForward->PushString(m_closeInfo.reason.c_str());
	m_client->pCloseForward->Execute(nullptr);
}

void WsErrorTaskContext::OnCompleted()
{
	m_client->pErrorForward->PushCell(m_client->m_websocket_handle);
	m_client->pErrorForward->PushString(m_errorInfo.reason.c_str());
	m_client->pErrorForward->Execute(nullptr);
}