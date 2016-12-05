# libRCON

## What is the RCON protocol

The RCON protocol is a protocol that allows a remote client to execute commands on a server.
This library implements the Minecraft RCON protocol. More information can be found [here](http://wiki.vg/RCON).

## Example

```c
#include <rcon/rcon.h>

#include <stdio.h>
#include <stdlib.h>

#define SERVER_HOSTNAME "localhost"
#define SERVER_PORT "22222"

#define SERVER_PASSWD "passwd"

int main(void) {
	// Initialize librcon
	if (!rconInitialize()) {
		return -1;
	}
	
	// Connect to the server.
	RCONclient *client = rconClientConnect(SERVER_HOSTNAME, SERVER_PORT);
	if (!client) {
		rconTerminate();
		return -1;
	}
	
	// Authenticate with the server.
	if (!rconClientAuthenticate(client, SERVER_PASSWD)) {
		rconClientDisconnect(client);
		rconTerminate();
		return -1;
	}
	
	// Run the help command
	const char *response = NULL;
	if (!rconClientCommand(client, "help", &response)) {
		rconClientDisconnect(client);
		rconTerminate();
		return -1;
	}
	
	// Print and free the response
	printf("%s\n", response);
	free(response);
	
	// Clean up used resources
	rconClientDisconnect(client);
	rconTerminate();
	
	return 0;
}
```