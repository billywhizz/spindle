FROM gcr.io/distroless/static:latest
COPY spin .
CMD ["./spin"]
