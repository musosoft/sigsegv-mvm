# CFuncReplace regression

From the repository root:

```sh
bash tests/run_func_replace_jump.sh
```

Runs GCC and Clang, each normally and with ASan+UBSan (fatal diagnostics).
It compiles the real `src/mem/patch.cpp`, `patch.h`, and `ByteBuf`, with small
engine-boundary doubles under `stubs/`. The protection double really switches
mapping permissions. No Source server or extension is loaded.

Coverage:
- Exact 14-byte `FF 25 00 00 00 00` plus little-endian uint64 address;
  zero/max addresses, 16 buffer alignments, undersized/null buffers, canaries.
- Fixed executable mappings 3 GiB and 32 TiB apart, in both directions.
  Mapping failures are failures, not skips. Addresses lie in ASan HighMem.
- Real `CFuncReplace::Init/Check` and `CPatch::Apply/UnApply`, 14-byte patch
  length/mask/protection, huge declared callback length without copying it,
  repeated apply/unapply, exact original bytes including neighboring canaries.
- All 15 non-stack GPRs, arithmetic/direction flags, stack pointer, and return
  path through a prologue-free callback, using an assembly state probe.

The doubles do not validate live address resolution, engine scanner integration,
other detours, concurrency while patching, or whether each server function has
14 safely overwritable bytes. These require separate server runtime validation.

`package_manifest.py PACKAGE_DIRECTORY` writes deterministic sorted JSON next to
the package (no timestamps), with per-file sizes/hashes and source provenance.
The server build is target metadata, not a claim that the server was tested.
