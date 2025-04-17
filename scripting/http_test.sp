#include <sourcemod>
#include <websocket>

public Plugin myinfo = {
	name = "HTTP Client Test",
	author = "ProjectSky",
	description = "Used for testing HTTP client",
	version = "1.0.0",
	url = "https://github.com/ProjectSky/sm-ext-websocket"
}

public void OnPluginStart()
{
	RegServerCmd("http_get", http_get);
	RegServerCmd("http_post_json", http_post_json);
	RegServerCmd("http_post_form", http_post_form);
	RegServerCmd("http_put", http_put);
	RegServerCmd("http_patch", http_patch);
	RegServerCmd("http_delete", http_delete);
}

Action http_get(int args)
{
	HttpRequest request = new HttpRequest("https://httpbin.org/get");
	request.AddHeader("User-Agent", "SourceMod HTTP Client");
	request.Get(OnHttpResponse);
	return Plugin_Handled;
}

Action http_post_json(int args)
{
	HttpRequest request = new HttpRequest("https://httpbin.org/post");
	
	// Create JSON payload
	YYJSONObject json = new YYJSONObject();
	json.SetString("name", "test");
	json.SetInt("age", 25);
	json.SetBool("active", true);
	
	// Add custom headers
	request.AddHeader("User-Agent", "SourceMod HTTP Client");
	
	// Send POST request with JSON body
	request.PostJson(json, OnHttpResponse);
	
	delete json;
	return Plugin_Handled;
}

Action http_post_form(int args)
{
	HttpRequest request = new HttpRequest("https://httpbin.org/post");
	
	// Add form parameters
	request.AppendFormParam("username", "john_doe");
	request.AppendFormParam("email", "john@example.com");
	request.AppendFormParam("age", "25");
	
	// Add custom headers
	request.AddHeader("User-Agent", "SourceMod HTTP Client");
	
	// Send POST request with form data
	request.PostForm(OnHttpResponse);
	return Plugin_Handled;
}

Action http_put(int args)
{
	HttpRequest request = new HttpRequest("https://httpbin.org/put");
	
	// Create JSON payload for PUT
	YYJSONObject json = new YYJSONObject();
	json.SetString("id", "123");
	json.SetString("name", "updated_name");
	json.SetBool("active", false);
	
	// Add custom headers
	request.AddHeader("User-Agent", "SourceMod HTTP Client");
	
	// Send PUT request with JSON body
	request.PutJson(json, OnHttpResponse);
	
	delete json;
	return Plugin_Handled;
}

Action http_patch(int args)
{
	HttpRequest request = new HttpRequest("https://httpbin.org/patch");
	
	// Create JSON payload for PATCH
	YYJSONObject json = new YYJSONObject();
	json.SetString("name", "partial_update");
	
	// Add custom headers
	request.AddHeader("User-Agent", "SourceMod HTTP Client");
	
	// Send PATCH request with JSON body
	request.PatchJson(json, OnHttpResponse);
	
	delete json;
	return Plugin_Handled;
}

Action http_delete(int args)
{
	HttpRequest request = new HttpRequest("https://httpbin.org/delete");
	request.AddHeader("User-Agent", "SourceMod HTTP Client");
	request.Delete(OnHttpResponse);
	return Plugin_Handled;
}

void OnHttpResponse(HttpRequest http, const char[] body, int statusCode, int bodySize, any value)
{
	// Print response details
	PrintToServer("Status Code: %d", statusCode);
	PrintToServer("Body Size: %d bytes", bodySize);
	
	// Print response headers
	char headerValue[256];
	if (http.GetResponseHeader("Content-Type", headerValue, sizeof(headerValue)))
	{
		PrintToServer("Content-Type: %s", headerValue);
	}
	
	// Print response body
	PrintToServer("Response Body: %s", body);
} 