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
<td width="50%" valign="middle" align="center">
<img src="assets/tiny-server-flow.png" width="100%" alt="Tiny C server flowchart showing startup, parent listener, worker request handling, and 200 or 405 responses" />
</td>
<td width="50%" valign="middle">

<h2><samp>› a tiny server, end to end</samp></h2>

<p>Tiny is a small C web server for learning how an HTTP request moves through a server. It binds only to <code>127.0.0.1:8080</code>, accepts a connection, and forks a short-lived worker to handle it.</p>

<p>The worker reads one request chunk with a five-second timeout and logs a sanitized request line. A <code>GET</code> receives the static HTML page; other methods receive <code>405 Method Not Allowed</code>. Each connection closes after the response.</p>

<hr />

<h2><samp>› run it</samp></h2>

<p>Requires a C compiler and <code>make</code>.</p>

<pre><code>make
./tiny</code></pre>

<p>Open <a href="http://127.0.0.1:8080">127.0.0.1:8080</a>, or check both response paths:</p>

<pre><code>curl -i http://127.0.0.1:8080/
curl -i -X POST http://127.0.0.1:8080/</code></pre>

<p>Stop the server with <code>Ctrl+C</code>.</p>

<hr />

<h2><samp>› build options</samp></h2>

<p>The Makefile enables compiler warnings, stack protection, and position-independent code. Linux builds also enable RELRO linker hardening. macOS skips Linux-specific linker flags.</p>

<p>Optional sanitizer build:</p>

<pre><code>cc -g -O1 -Wall -Wextra \
  -fsanitize=address,undefined \
  -fno-omit-frame-pointer \
  -o tiny-asan tiny.c
./tiny-asan</code></pre>

<hr />

<h2><samp>› Fedora systemd</samp></h2>

<p>The included <code>tiny.service</code> runs as a dynamic unprivileged user in a systemd sandbox. After building on Fedora:</p>

<pre><code>sudo install -m 0755 tiny /usr/local/bin/tiny
sudo install -m 0644 tiny.service /etc/systemd/system/tiny.service
sudo systemctl daemon-reload
sudo systemctl enable --now tiny
systemctl status tiny</code></pre>

<hr />

<h2><samp>› network boundary</samp></h2>

<p>The server does not serve files or bind to your LAN address. For public access, use a Cloudflare Tunnel and keep port <code>8080</code> closed at the router.</p>
</td>
</tr>
</table>

## <samp>› limits and deployment</samp>

This is a learning project, not a general-purpose HTTP server. It supports IPv4 and `GET`, reads only one request chunk, ignores paths, closes each connection, and forks per connection. It should stay on loopback. The server trusts `CF-Connecting-IP` only for logging, so use that value only when the tunnel is the sole route to the server.

<p align="center"><samp>C · POSIX sockets · HTTP · systemd</samp></p>
