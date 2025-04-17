#include "extension.h"

HttpRequest::HttpRequest(const std::string &url) : m_httpclient(true)
{
	m_request = m_httpclient.createRequest(url);
}

HttpRequest::~HttpRequest() 
{
	if (pResponseForward) forwards->ReleaseForward(pResponseForward);
}

void HttpRequest::SetBody(const std::string &body)
{
	m_request->body = body;
}

void HttpRequest::SetJsonBody(YYJsonWrapper* json)
{
	if (!json) return;

	char* jsonStr;

	if (json->m_pDocument_mut) {
		jsonStr = yyjson_mut_write(json->m_pDocument_mut.get(), 0, nullptr);
	} else {
		jsonStr = yyjson_write(json->m_pDocument.get(), 0, nullptr);
	}

	if (jsonStr)
	{
		m_request->body = jsonStr;
		m_request->extraHeaders["Content-Type"] = "application/json";
		free(jsonStr);
	}
}

void HttpRequest::AddHeader(const std::string &key, const std::string &value)
{
	m_request->extraHeaders[key] = value;
}

void HttpRequest::SetTimeout(int timeout)
{
	m_request->connectTimeout = timeout;
}

int HttpRequest::GetTimeout()
{
	return m_request->connectTimeout;
}

void HttpRequest::SetFollowRedirect(bool follow) 
{
	m_request->followRedirects = follow;
}

bool HttpRequest::GetFollowRedirect() 
{
	return m_request->followRedirects;
}

void HttpRequest::SetCompression(bool compress)
{
	m_request->compress = compress;
}

bool HttpRequest::GetCompression()
{
	return m_request->compress;
}

void HttpRequest::SetMaxRedirects(int maxRedirects)
{
	m_request->maxRedirects = maxRedirects;
}

int HttpRequest::GetMaxRedirects()
{
	return m_request->maxRedirects;
}

void HttpRequest::SetVerbose(bool verbose)
{
	m_request->verbose = verbose;
}

bool HttpRequest::GetVerbose()
{
	return m_request->verbose;
}

void HttpRequest::onResponse(const ix::HttpResponsePtr response, IPluginFunction *callback, cell_t value) 
{
	if (!pResponseForward || !pResponseForward->GetFunctionCount())
	{
		return;
	}

	if (response) {
		std::lock_guard<std::mutex> lock(m_headersMutex);
		m_responseHeaders = response->headers;
	}

	HttpResponseTaskContext *context = new HttpResponseTaskContext(this, response, callback, value);
	g_WebsocketExt.AddTaskToQueue(context);
}

void HttpResponseTaskContext::OnCompleted()
{
	HandleError err;
	HandleSecurity sec(nullptr, myself->GetIdentity());

	m_client->pResponseForward->PushCell(m_client->m_httpclient_handle);
	m_client->pResponseForward->PushString(m_response->body.c_str());
	m_client->pResponseForward->PushCell(m_response->statusCode);
	m_client->pResponseForward->PushCell(m_response->body.size());
	m_client->pResponseForward->PushCell(m_value);
	m_client->pResponseForward->Execute(nullptr);

	handlesys->FreeHandle(m_client->m_httpclient_handle, &sec);
}

bool HttpRequest::Get(IPluginFunction *callback, cell_t value)
{
	m_request->verb = "GET";
	return m_httpclient.performRequest(m_request, 
		[this, callback, value](const ix::HttpResponsePtr& response) {
			onResponse(response, callback, value);
		});
}

bool HttpRequest::PostJson(YYJsonWrapper* json, IPluginFunction *callback, cell_t value)
{
	if (!json) return false;
	
	m_request->verb = "POST";
	SetJsonBody(json);
	return m_httpclient.performRequest(m_request, 
		[this, callback, value](const ix::HttpResponsePtr& response) {
			onResponse(response, callback, value);
		});
}

bool HttpRequest::PutJson(YYJsonWrapper* json, IPluginFunction *callback, cell_t value)
{
	if (!json) return false;
	
	m_request->verb = "PUT";
	SetJsonBody(json);
	return m_httpclient.performRequest(m_request, 
		[this, callback, value](const ix::HttpResponsePtr& response) {
			onResponse(response, callback, value);
		});
}

bool HttpRequest::PatchJson(YYJsonWrapper* json, IPluginFunction *callback, cell_t value)
{
	if (!json) return false;
	
	m_request->verb = "PATCH";
	SetJsonBody(json);
	return m_httpclient.performRequest(m_request, 
		[this, callback, value](const ix::HttpResponsePtr& response) {
			onResponse(response, callback, value);
		});
}

bool HttpRequest::PostForm(IPluginFunction *callback, cell_t value)
{
	m_request->verb = "POST";
	m_request->body = BuildFormData();
	m_request->extraHeaders["Content-Type"] = "application/x-www-form-urlencoded";
	return m_httpclient.performRequest(m_request, 
		[this, callback, value](const ix::HttpResponsePtr& response) {
			onResponse(response, callback, value);
		});
}

bool HttpRequest::Delete(IPluginFunction *callback, cell_t value)
{
	m_request->verb = "DELETE";
	return m_httpclient.performRequest(m_request, 
		[this, callback, value](const ix::HttpResponsePtr& response) {
			onResponse(response, callback, value);
		});
}

std::string HttpRequest::BuildFormData()
{
	std::string formData;
	bool first = true;
	
	for (const auto& param : m_formParams) {
		if (!first) {
			formData += "&";
		}
		formData += m_httpclient.urlEncode(param.first) + "=" + m_httpclient.urlEncode(param.second);
		first = false;
	}
	
	return formData;
}

void HttpRequest::AppendFormParam(const std::string &key, const std::string &value)
{
	m_formParams[key] = value;
}

const std::string& HttpRequest::GetResponseHeader(const std::string& key) const 
{
	static const std::string empty;
	std::lock_guard<std::mutex> lock(m_headersMutex);
	auto it = m_responseHeaders.find(key);
	return it != m_responseHeaders.end() ? it->second : empty;
}

bool HttpRequest::HasResponseHeader(const std::string& key) const
{
	std::lock_guard<std::mutex> lock(m_headersMutex);
	return m_responseHeaders.find(key) != m_responseHeaders.end();
}

const ix::WebSocketHttpHeaders& HttpRequest::GetResponseHeaders() const 
{ 
	std::lock_guard<std::mutex> lock(m_headersMutex);
	return m_responseHeaders; 
}