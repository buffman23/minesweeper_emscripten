FROM emscripten/emsdk AS base

# Install dependencies
FROM base AS deps
WORKDIR /emsdk/upstream/emscripten
RUN npm install