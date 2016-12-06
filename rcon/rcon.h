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

/**
 * \brief Have the client authenticate with the server.
 * This is nessesary in order to start sending commands to the server.
 * 
 * \param client The client.
 * \param password The password to use.
 * \returns If the client successfully authenticated with the server.
 */
RCONbool rconClientAuthenticate(RCONclient *client, const char *password);

/**
 * \brief Runs a command on the server.
 * 
 * \param client The client.
 * \param command The command to run.
 * \param response Where to store the response (It will have to be free'd using free()).
 * \returns If the command was successfully sent.
 */
RCONbool rconClientCommand(RCONclient *client, const char *command, char **response);

/**
 * \brief Connects to a RCON server.
 * 
 * \param serverName A server's domain name (example.com) or an ip (192.168.1.62, 2001:0DB8:AC10:FE01::).
 * \param port The port that the RCON server is listening on.
 * \returns A pointer to a client if successfull, NULL otherwise.
 */
RCONclient *rconClientConnect(const char *serverName, const char *port);

/**
 * \brief Disconnects from a server.
 * 
 * \param client The client to disconnect.
 */
void rconClientDisconnect(RCONclient *client);

/**
 * \brief Gets the last error that occured.
 * 
 * \returns The last error.
 */
RCONerror rconGetLastError(void);

/**
 * \brief Initializes libRCON.
 * 
 * \note rconInitializeThread() is called.
 */
RCONbool rconInitialize(void);

/**
 * \brief Initializes TLS for the current thread.
 * 
 * \note rconInitialize() must be called first.
 */
RCONbool rconInitializeThread(void);

/**
 * \brief Terminates libRCON.
 * 
 * \note rconTerminateThread() is called.
 */
void rconTerminate(void);

/**
 * \brief Terminates TLS for the current thread.
 * 
 * \note This must be called before rconTerminate().
 */
void rconTerminateThread(void);

#endif
