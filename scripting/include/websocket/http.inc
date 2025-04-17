// HTTP FUNCTIONS ARE NOT COMPLETED, THEY ARE STILL BEING TESTED

// Define a typeset for HTTP response callbacks
typeset ResponseCallback
{
  /**
  * Function called when an HTTP response is received
  *
  * @param http        HTTP request object
  * @param body        Response body
  * @param statusCode  HTTP status code
  * @param bodySize    Size of the response body
  * @param value       Value passed to the HTTP method
  */
  function void (HttpRequest http, const char[] body, int statusCode, int bodySize, any value);
}

methodmap HttpRequest < Handle
{
  /**
  * Create a new HTTP request
  *
  * @param url         Request URL
  */
  public native HttpRequest(const char[] url);

  /**
  * Performs a GET request
  * GET requests should not have a body, only query parameters in URL
  *
  * @param fResponse   Function to call when an HTTP response is received
  * @param value       Value to pass to the callback
  * @return            True if the request was sent successfully, false otherwise
  */
  public native bool Get(ResponseCallback fResponse, any value = 0);

  /**
  * Performs a POST request with JSON body
  * POST requests typically send data to create a new resource
  *
  * @param json        JSON data to send in request body
  * @param fResponse   Function to call when an HTTP response is received
  * @param value       Value to pass to the callback
  * @return            True if the request was sent successfully, false otherwise
  */
  public native bool PostJson(const YYJSON json, ResponseCallback fResponse, any value = 0);

  /**
  * Performs a POST request with accumulated form parameters
  * Uses application/x-www-form-urlencoded format
  *
  * @param fResponse   Function to call when an HTTP response is received
  * @param value       Value to pass to the callback
  * @return            True if the request was sent successfully, false otherwise
  */
  public native bool PostForm(ResponseCallback fResponse, any value = 0);

  /**
  * Performs a PUT request with JSON body
  * PUT requests typically update an entire resource
  *
  * @param json        JSON data to send in request body
  * @param fResponse   Function to call when an HTTP response is received
  * @param value       Value to pass to the callback
  * @return            True if the request was sent successfully, false otherwise
  */
  public native bool PutJson(const YYJSON json, ResponseCallback fResponse, any value = 0);

  /**
  * Performs a PATCH request with JSON body
  * PATCH requests typically perform partial updates to a resource
  *
  * @param json        JSON data containing the fields to update
  * @param fResponse   Function to call when an HTTP response is received
  * @param value       Value to pass to the callback
  * @return            True if the request was sent successfully, false otherwise
  */
  public native bool PatchJson(const YYJSON json, ResponseCallback fResponse, any value = 0);

  /**
  * Performs a DELETE request
  * DELETE requests typically don't have a body
  *
  * @param fResponse   Function to call when an HTTP response is received
  * @param value       Value to pass to the callback
  * @return            True if the request was sent successfully, false otherwise
  */
  public native bool Delete(ResponseCallback fResponse, any value = 0);

  /**
  * Append a form parameter to the request
  * Multiple calls will accumulate parameters for the next form submission
  *
  * @param key         Parameter key
  * @param value       Parameter value
  * @return            True if parameter was added successfully
  */
  public native bool AppendFormParam(const char[] key, const char[] value);

  /**
  * Add a header to the HTTP request
  *
  * @param key         Header key
  * @param value       Header value
  */
  public native void AddHeader(const char[] key, const char[] value);

  /**
  * Get a response header from the HTTP request
  *
  * @param key         Header key
  * @param value       Buffer to store the header value
  * @param maxLength   Maximum length of the value buffer
  * @return            True if the header was found, false otherwise
  */
  public native bool GetResponseHeader(const char[] key, char[] value, int maxLength);

  /**
  * Check if a response header exists in the HTTP request
  *
  * @param key         Header key
  * @return            True if the header exists, false otherwise
  */
  public native bool HasResponseHeader(const char[] key);

  property int Timeout {
    public native get();
    public native set(int timeout);
  }

  property bool FollowRedirect {
    public native get();
    public native set(bool follow);
  }

  property bool Compression {
    public native get();
    public native set(bool compress);
  }

  property int MaxRedirects {
    public native get();
    public native set(int maxRedirects);
  }

  property bool Verbose {
    public native get();
    public native set(bool verbose);
  }
}