#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef TINY_PORT
#define TINY_PORT 8080
#endif

static const char body[] =
    "<!doctype html><html lang=\"en\"><meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>Tiny C Server</title><body><main>"
    "<h1>It works.</h1><p>Served by a tiny C program.</p>"
    "</main></body></html>\n";

static void clean(char *dst, size_t cap, const char *src) {
    size_t i = 0;
    if (!cap) return;
    for (; i + 1 < cap && src[i] && src[i] != '\r' && src[i] != '\n'; ++i) {
        unsigned char c = (unsigned char)src[i];
        dst[i] = (c >= 32 && c < 127) ? (char)c : '?';
    }
    dst[i] = '\0';
}

static int write_all(int fd, const char *buf, size_t len) {
    while (len) {
        ssize_t n = write(fd, buf, len);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        buf += n;
        len -= (size_t)n;
    }
    return 0;
}

static void respond(int client, const struct sockaddr_in *peer) {
    alarm(5);
    char req[2048];
    ssize_t n;
    do { n = read(client, req, sizeof req - 1); } while (n < 0 && errno == EINTR);
    if (n <= 0) return;
    req[n] = '\0';

    char peer_ip[INET_ADDRSTRLEN] = "unknown";
    char client_ip[64] = "-";
    char req_line[120] = "";
    (void)inet_ntop(AF_INET, &peer->sin_addr, peer_ip, sizeof peer_ip);
    clean(req_line, sizeof req_line, req);

    const char *header = strcasestr(req, "\r\ncf-connecting-ip:");
    if (header) {
        header += strlen("\r\ncf-connecting-ip:");
        while (*header == ' ' || *header == '\t') ++header;
        clean(client_ip, sizeof client_ip, header);
    }
    fprintf(stderr, "peer=%s client=%s req=\"%s\"\n", peer_ip, client_ip, req_line);

    if (n >= 4 && memcmp(req, "GET ", 4) == 0) {
        char response[1024];
        int len = snprintf(response, sizeof response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "Cache-Control: public, max-age=300\r\n"
            "X-Content-Type-Options: nosniff\r\n\r\n%s",
            sizeof body - 1, body);
        if (len > 0 && (size_t)len < sizeof response)
            (void)write_all(client, response, (size_t)len);
    } else {
        static const char method_not_allowed[] =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Allow: GET\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        (void)write_all(client, method_not_allowed, sizeof method_not_allowed - 1);
    }
}

int main(void) {
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int one = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) < 0) {
        perror("setsockopt"); close(server); return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TINY_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(server, (struct sockaddr *)&addr, sizeof addr) < 0 || listen(server, 64) < 0) {
        perror("bind/listen (is port 8080 already in use?)"); close(server); return 1;
    }
    fprintf(stderr, "tiny listening on http://127.0.0.1:%d\n", TINY_PORT);

    for (;;) {
        struct sockaddr_in peer;
        socklen_t peer_len = sizeof peer;
        int client = accept(server, (struct sockaddr *)&peer, &peer_len);
        if (client < 0) {
            if (errno == EINTR) continue;
            perror("accept"); continue;
        }
        pid_t child = fork();
        if (child == 0) {
            close(server);
            respond(client, &peer);
            close(client);
            _exit(0);
        }
        if (child < 0) perror("fork");
        close(client);
    }
}
