import { serve, Response } from 'example/http4.js'

serve((req) => {
  console.log(req.method)
  console.log(req.path)
  console.log(req.headers)
  const res = new Response('Hello, World!')
})
