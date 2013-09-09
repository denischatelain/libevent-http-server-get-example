#include <evhttp.h>

/**
* This is a skeleton for showing how to use libevent's http features
* It only implements an http server answering to GET requests.
* Features:
*   - listens on the loopback, on port 32001
*   - has only one valid url, "/testing"
*   - takes one parameter, 'q', and outputs back its value
*/


/**
 *
 * @param *request the opaque data structure containing the request infos
 * @param *privParams global parameters set when the callback was associated
 * @return nothing
 */
void testing (struct evhttp_request *request, void *privParams) {
	struct evbuffer *buffer;
	struct evkeyvalq headers;
	const char *q;
	// Parse the query for later lookups
	evhttp_parse_query (evhttp_request_get_uri (request), &headers);

	// lookup the 'q' GET parameter 
	q = evhttp_find_header (&headers, "q");

	// Create an answer buffer where the data to send back to the browser will be appened
	buffer = evbuffer_new ();
	evbuffer_add (buffer, "coucou !", 8);
	evbuffer_add_printf (buffer, "%s", q);

	// Add a HTTP header, an application/json for the content type here
	evhttp_add_header (evhttp_request_get_output_headers (request),
			"Content-Type", "application/json");

	// Tell we're done and data should be sent back
	evhttp_send_reply(request, HTTP_OK, "OK", buffer);

	// Free up stuff
	evhttp_clear_headers (&headers);

	evbuffer_free (buffer);

	return;
}

/**
 *
 * @desc the "404 Not Found" callback; catches any unrecognized path requested
 * @return nothing
 */
void notfound (struct evhttp_request *request, void *params) {
	evhttp_send_error(request, HTTP_NOTFOUND, "Not Found"); 
}

int main (void) {
	struct event_base *ebase;
	struct evhttp *server;

	// Create a new event handler
	ebase = event_base_new ();;

	// Create a http server using that handler
	server = evhttp_new (ebase);

	// Limit serving GET requests
	evhttp_set_allowed_methods (server, EVHTTP_REQ_GET);

	// Set a test callback, /testing
	evhttp_set_cb (server, "/testing", testing, 0);

	// Set the callback for anything not recognized
	evhttp_set_gencb (server, notfound, 0);

	// Listen locally on port 32001
	if (evhttp_bind_socket (server, "127.0.0.1", 32001) != 0)
		errx (1, "Could not bind to 127.0.0.1:32001");

	// Start processing queries
	event_base_dispatch(ebase);

	// Free up stuff
	evhttp_free (server);

	event_base_free (ebase);
}
