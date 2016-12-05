#include <rcon/rcon.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#   pragma comment(lib, "ws2_32.lib")
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   include <winsock2.h>
#	include <WS2tcpip.h>

#   define htole32(x) (x)
#	define le32toh(x) (x)

typedef DWORD tls_index_t;

#   define _tlsGetValue(index) TlsGetValue(index)
#   define _tlsSetValue(index, value) TlsSetValue(index, value)

#else
#   include <arpa/inet.h>
#   include <endian.h>
#   include <netdb.h>
#   include <sys/socket.h>
#   include <unistd.h>

typedef int SOCKET;
#	define INVALID_SOCKET (-1)
#endif

#define SERVERDATA_RESPONSE_VALUE (0)
#define SERVERDATA_AUTH_RESPONSE (2)
#define SERVERDATA_EXECCOMMAND (2)
#define SERVERDATA_AUTH (3)

struct RCONclient {
	SOCKET sock;
	int pktId;
};

static tls_index_t _lastErrorTLSIndex;

static void _setLastError(RCONerror error) {
	RCONerror *lastError = (RCONerror *) _tlsGetValue(_lastErrorTLSIndex);
	*lastError = error;
}

static RCONbool _recvPacket(RCONclient *client, int *id, int *type, char **body) {
	char nullByte;
	int packetLength;

	if (!recv(client->sock, (char *)&packetLength, 4, 0)) {
		_setLastError(RCON_ERROR_PLATFORM);
		return RCON_FALSE;
	}
	packetLength = le32toh(packetLength);

	if (!recv(client->sock, (char *) id, 4, 0)) {
		_setLastError(RCON_ERROR_PLATFORM);
		return RCON_FALSE;
	}
	*id = le32toh(*id);

	if (!recv(client->sock, (char *) type, 4, 0)) {
		_setLastError(RCON_ERROR_PLATFORM);
		return RCON_FALSE;
	}
	*type = le32toh(*type);

	*body = (char *) malloc(packetLength - 9);
	if (!*body) {
		_setLastError(RCON_ERROR_OUT_OF_MEMORY);
		return RCON_FALSE;
	}
	if (!recv(client->sock, *body, packetLength - 9, 0)) {
		_setLastError(RCON_ERROR_PLATFORM);
		return RCON_FALSE;
	}

	if (!recv(client->sock, &nullByte, 1, 0)) {
		_setLastError(RCON_ERROR_PLATFORM);
		return RCON_FALSE;
	}

	return RCON_TRUE;
}

static struct addrinfo *_resolveHostname(const char *hostname, const char *port) {
	struct addrinfo *result = NULL, hints;
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	status = getaddrinfo(hostname, port, &hints, &result);
	if (status) {
		_setLastError(RCON_ERROR_UNRESOLVED_HOSTNAME);
		return NULL;
	}

	return result;
}

static RCONbool _sendPacket(RCONclient *client, int type, const char *body) {
	int packetLength = 10 + (int) strlen(body);
	unsigned char *packet = malloc(packetLength + 4);
	if (!packet) {
		_setLastError(RCON_ERROR_OUT_OF_MEMORY);
		return RCON_FALSE;
	}

	memset(packet, 0, packetLength + 4);

	((int *) packet)[0] = htole32(packetLength);
	((int *) packet)[1] = htole32(client->pktId++);
	((int *) packet)[2] = htole32(type);

	strcpy(packet + 12, body);

	if (client->pktId < 0) {
		client->pktId = 1;
	}

	if (!send(client->sock, packet, packetLength + 4, 0)) {
		_setLastError(RCON_ERROR_PLATFORM);
		free(packet);
		return RCON_FALSE;
	}

	free(packet);

	return RCON_TRUE;
}

RCONbool rconClientAuthenticate(RCONclient *client, const char *password) {
	RCONbool status;
	int id, type;
	char *body = NULL;

	status = _sendPacket(client, SERVERDATA_AUTH, password);
	if (!status) {
		return RCON_FALSE;
	}

	status = _recvPacket(client, &id, &type, &body);
	if (!status) {
		if (body) {
			free(body);
		}
		return RCON_FALSE;
	}
	free(body);

	if (id == -1) {
		_setLastError(RCON_ERROR_AUTHENTICATION_FAILURE);
		return RCON_FALSE;
	}

	return RCON_TRUE;
}

RCONbool rconClientCommand(RCONclient *client, const char *command, char **response) {
	RCONbool status;
	int id, type;

	status = _sendPacket(client, SERVERDATA_EXECCOMMAND, command);
	if (!status) {
		return RCON_FALSE;
	}

	status = _recvPacket(client, &id, &type, response);
	if (!status) {
		if (*response) {
			free(*response);
		}
		return RCON_FALSE;
	}

	return RCON_TRUE;
}

RCONclient *rconClientConnect(const char *serverName, const char *port) {
	struct addrinfo *result, *ptr;
	RCONclient *client;
	int status;

	client = (RCONclient *) malloc(sizeof(RCONclient));
	if (!client) {
		_setLastError(RCON_ERROR_OUT_OF_MEMORY);
		return NULL;
	}

	result = _resolveHostname(serverName, port);
	if (!result) {
		free(client);
		return NULL;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		client->sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (client->sock == INVALID_SOCKET) {
			continue;
		}

		status = connect(client->sock, ptr->ai_addr, (int) ptr->ai_addrlen);
		if (status == SOCKET_ERROR) {
			closesocket(client->sock);
			client->sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (client->sock == INVALID_SOCKET) {
		_setLastError(RCON_ERROR_CONNECTION_FAILURE);
		free(client);
		return NULL;
	}

	return client;
}

void rconClientDisconnect(RCONclient *client) {
	int status = 0;
#ifdef _WIN32
	status = shutdown(client->sock, SD_BOTH);
	if (!status) {
		closesocket(client->sock);
	}
#else
	status = shutdown(client->sock, SHUT_RDWR);
	if (!status) {
		close(client->sock);
	}
#endif
	free(client);
}

RCONerror rconGetLastError(void) {
	RCONerror *storedError = (RCONerror *) _tlsGetValue(_lastErrorTLSIndex);
	RCONerror error = *storedError;
	*storedError = RCON_ERROR_NONE;

	return error;
}

RCONbool rconInitialize(void) {
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1, 1), &wsaData);
	_lastErrorTLSIndex = TlsAlloc();
#else

#endif
	rconInitializeThread();

	return RCON_TRUE;
}

RCONbool rconInitializeThread(void) {
	RCONerror *lastError = (RCONerror *) malloc(sizeof(RCONerror));
	*lastError = RCON_ERROR_NONE;
	_tlsSetValue(_lastErrorTLSIndex, lastError);

	return RCON_TRUE;
}

void rconTerminate(void) {
#ifdef _WIN32
	WSACleanup();
#endif
	rconTerminateThread();
}

void rconTerminateThread(void) {
	free(_tlsGetValue(_lastErrorTLSIndex));
}
