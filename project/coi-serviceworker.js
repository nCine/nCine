// Enables cross-origin isolation (needed by `SharedArrayBuffer` and Emscripten pthreads) on hosts
// that cannot set the COOP/COEP HTTP headers, like GitHub Pages.
// This file runs in two contexts:
// - in the page, where it registers itself as a service worker;
// - in the service worker, where it adds the headers to every response.
if (typeof window === 'undefined') {
  self.addEventListener('install', () => self.skipWaiting());
  self.addEventListener('activate', (event) => event.waitUntil(self.clients.claim()));

  self.addEventListener('fetch', (event) => {
    const request = event.request;
    // Some browsers throw on this particular combination
    if (request.cache === 'only-if-cached' && request.mode !== 'same-origin')
      return;

    event.respondWith(
      fetch(request).then((response) => {
        // Opaque responses cannot be modified
        if (response.status === 0)
          return response;

        const headers = new Headers(response.headers);
        headers.set('Cross-Origin-Embedder-Policy', 'require-corp');
        headers.set('Cross-Origin-Opener-Policy', 'same-origin');
        return new Response(response.body, {
          status: response.status,
          statusText: response.statusText,
          headers: headers
        });
      }).catch((e) => console.error(e))
    );
  });
} else if (!window.crossOriginIsolated && window.isSecureContext && 'serviceWorker' in navigator) {
  const script = document.currentScript;
  navigator.serviceWorker.register(script.src).then((registration) => {
    registration.addEventListener('updatefound', () => window.location.reload());
    // The page is not controlled yet on the first visit, reload it to get the headers
    if (registration.active && !navigator.serviceWorker.controller)
      window.location.reload();
  }, (e) => console.error('Cross-origin isolation service worker registration failed: ', e));
}
