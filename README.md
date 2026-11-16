<div align="center">

<h1><code>Tiny C Web Server</code></h1>

<samp>A small, loopback-only HTTP server built to learn how a web server works.</samp>

<br />

<img src="https://img.shields.io/badge/C-POSIX-A8B9CC?style=for-the-badge&logo=c&logoColor=white" alt="C and POSIX" />
<img src="https://img.shields.io/badge/HTTP-Static%20server-4285F4?style=for-the-badge" alt="Static HTTP server" />
<img src="https://img.shields.io/badge/Binding-127.0.0.1-2E8B57?style=for-the-badge" alt="Loopback only" />

</div>

<br />

<p align="center"><samp>ʟᴇᴀʀɴɪɴɢ ᴘʀᴏᴊᴇᴄᴛ · ꜰᴏʀᴋ-ᴘᴇʀ-ᴄᴏɴɴᴇᴄᴛɪᴏɴ · ᴜꜱᴇʀ-ʀᴜɴ ʟᴏᴄᴀʟʟʏ</samp></p>

---

<table>
<tr>
<td width="56%" valign="middle">

<h2><samp>› what it does</samp></h2>

<p>Tiny binds to <code>127.0.0.1:8080</code> and serves a small HTML page for <code>GET</code> requests. Other methods receive <code>405 Method Not Allowed</code>. It forks a worker for each connection, applies a five-second request timeout, and writes sanitized request details to stderr.</p>

<h2><samp>› run it</samp></h2>

<p>Requires a C compiler and <code>make</code>.</p>

<pre><code>make
./tiny</code></pre>

<p>Then open <a href="http://127.0.0.1:8080">127.0.0.1:8080</a>, or check it with curl:</p>

<pre><code>curl -i http://127.0.0.1:8080/
curl -i -X POST http://127.0.0.1:8080/</code></pre>

</td>
<td width="44%" align="center" valign="top">
<img src="assets/tiny-server-flow.png" width="340" alt="Vertical flowchart showing the Tiny C web server startup, parent listener, forked child request handling, and 200 or 405 responses" />
</td>
</tr>
</table>

## <samp>› request flow</samp>

The diagram shows the lifecycle of a request: the parent accepts a client, forks a worker, and closes its copy of the client socket. The worker reads one request chunk, logs it, returns a response, then exits. The server handles one request per connection and ignores the requested URL path.

## <samp>› build options</samp>

The Makefile enables compiler warnings, stack protection, and position-independent code. On Linux it also enables RELRO linker hardening. On macOS it skips Linux-specific linker flags so the project can be built locally.

```sh
# Optional sanitizer build
cc -g -O1 -Wall -Wextra -fsanitize=address,undefined \
  -fno-omit-frame-pointer -o tiny-asan tiny.c
./tiny-asan
```

## <samp>› Fedora systemd</samp>

The included [`tiny.service`](tiny.service) runs the server as a dynamic unprivileged user with a systemd sandbox. On Fedora, after building:

```sh
sudo install -m 0755 tiny /usr/local/bin/tiny
sudo install -m 0644 tiny.service /etc/systemd/system/tiny.service
sudo systemctl daemon-reload
sudo systemctl enable --now tiny
systemctl status tiny
```

## <samp>› limits and deployment</samp>

This is a learning project, not a general-purpose HTTP server. It supports IPv4 and `GET`, reads only one request chunk, ignores paths, closes each connection, and forks per connection. It should stay on loopback. For public access, use a Cloudflare Tunnel as described in the local plan; do not forward port 8080 from your router. The server trusts `CF-Connecting-IP` only for logging, so use that value only when the tunnel is the sole route to the server.

<p align="center"><samp>C · POSIX sockets · HTTP · systemd</samp></p>
