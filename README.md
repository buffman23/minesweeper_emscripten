A WebAssembly minesweeper field data structure for running minesweeper in the browser.
Written in C++ and compilted using emscripten.

```typescript
// typescript interface
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  clone(): this;
}
export interface MineField extends ClassHandle {
  readonly seed: number;
  sweep(x: number, y: number): number;
  flag(x: number, y: number): number;
  clear(): void;
  revealGrid(): void;
  getWidth(): number;
  getHeight(): number;
  getSeed(): number;
  getAt(x: number, y: number): number;
  toString(): string;
}

interface EmbindModule {
  MineField: {
    new(width: number, height: number): MineField;
    new(width: number, height: number, num_of_mines: number, seed: number, spawn_guard_radius: number): MineField;
  };
}
```
![ms_sample](https://github.com/user-attachments/assets/661938d9-4283-4835-a349-2aa46e1aab57)
