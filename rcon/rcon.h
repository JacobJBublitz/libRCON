#ifndef RCON_RCON_H
#define RCON_RCON_H

typedef int RCONbool;
#define RCON_TRUE 1
#define RCON_FALSE 0

typedef struct RCONclient RCONclient;

typedef enum {
	RCON_ERROR_NONE,
	RCON_ERROR_AUTHENTICATION_FAILURE,
	RCON_ERROR_CONNECTION_FAILURE,
	RCON_ERROR_OUT_OF_MEMORY,
	RCON_ERROR_PLATFORM,
	RCON_ERROR_UNRESOLVED_HOSTNAME
} RCONerror;

RCONbool rconClientAuthenticate(RCONclient *client, const char *password);
RCONbool rconClientCommand(RCONclient *client, const char *command, char **response);
RCONclient *rconClientConnect(const char *serverName, const char *port);
void rconClientDisconnect(RCONclient *client);

RCONerror rconGetLastError(void);

RCONbool rconInitialize(void);
RCONbool rconInitializeThread(void);
void rconTerminate(void);
void rconTerminateThread(void);

#endif
