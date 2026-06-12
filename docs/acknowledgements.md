# Acknowledgements

AsmKit builds on public work from the following projects.

## LLVM

AsmKit target data is derived from LLVM target records. The checked-in generated
files are produced by the project's modified TableGen-based generator,
`asmkit-tblgen`, and are marked `Apache-2.0 WITH LLVM-exception`.

- Project: <https://llvm.org/>
- License text: <https://llvm.org/LICENSE.txt>

## Zydis

Some instruction corpus tests are converted by project scripts from Zydis test
material.

- Project: <https://zydis.re/>
- Repository: <https://github.com/zyantific/zydis>

## Capstone

Some instruction corpus tests are converted by project scripts from Capstone
test material.

- Project: <https://www.capstone-engine.org/>
- Repository: <https://github.com/capstone-engine/capstone>

## AsmJit

Some instruction corpus tests are converted by project scripts from AsmJit test
material.

- Project: <https://asmjit.com/>
- Repository: <https://github.com/asmjit/asmjit>

## Notes

The converted tests are used to validate AsmKit behavior. They are not API
contracts for the upstream projects.
