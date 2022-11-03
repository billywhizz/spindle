import { serve, Response } from 'example/http3.js'

serve({
  fetch: () => {
    return new Response('Hello, World!')
  }
})
