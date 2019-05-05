# Device Control v1

## 2. Handle the hello message

1. Is realm registered?
   - If realm is _not_ registered abort session with error ERR_NO_SUCH_REALM
   - If realm is registered set the session state to ANNOUNCED, publish realm
     and details to potential subscribers, start a timer for the welcome message
     request on the request-reply queue and hope someone is interessted.
2. On welcome message timeout, abort the session with error ERR_TO_BE_DEFINED
3. On welcome message received on the request-reply queue with matching realm,
   send welcome message with given session ID and details to the websocket
   client.
   - If message was sent successfully, set session state to REGISTERED.
   - On error set session state to CLOSED.
