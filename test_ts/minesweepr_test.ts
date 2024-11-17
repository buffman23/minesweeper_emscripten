import MainModuleFactory, { MainModule, MineField, Tile } from '../build-emscripten/minesweeper';
import * as fs from 'fs/promises';
//const ModuleFactory = require('../build-emscripten/minesweeper');
//import Module from './library';

async function loadWasmModule(): Promise<MainModule> {
    // Read the .wasm file as a buffer
    const wasmBuffer = await fs.readFile('../build-emscripten/minesweeper.wasm');

    // Pass the buffer to the Emscripten factory
    const module = await MainModuleFactory({
        wasmBinary: wasmBuffer,
    });

    return module as MainModule;
}

async function main() {
    // Initialize the Wasm module
     // Read the .wasm file as a buffer
     const module = await loadWasmModule();

    // Use the functions exposed by the Wasm module
    const minefield = new module.MineField(10, 10);
    console.log("Minefield dimensions:", minefield.getWidth(), "x", minefield.getHeight());

    const positions: Tile[] = minefield.sweep(0, 0);
    console.log("isArray = " + Array.isArray(positions));
    console.log("Sweep result:", positions);

    console.log(minefield.toString());

    minefield.delete();
}

main();