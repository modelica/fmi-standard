# FMI Platform Tuple Registry

List of all standardized `<arch>` and `<sys>` names for the FMI "platform tuple" (`<arch>-<sys>{-<abi>}`), as defined in [Platform Tuple Definition](docs/2_5_fmu_distribution.adoc#platform-tuple-definition) of the core specification.

## Architecture (`<arch>`)

| Name | Description |
|---|---|
| `aarch32` | ARM 32-bit Architecture |
| `aarch64` | ARM 64-bit Architecture |
| `loongarch32` | LoongArch 32-bit Architecture |
| `loongarch64` | LoongArch 64-bit Architecture |
| `ppc32` | PowerPC 32-bit |
| `ppc64` | PowerPC 64-bit |
| `riscv32` | RISC-V 32-bit |
| `riscv64` | RISC-V 64-bit |
| `x86` | Intel/AMD x86 32-bit |
| `x86_64` | Intel/AMD x86 64-bit |

## Operating system (`<sys>`)

| Name | Description |
|---|---|
| `darwin` | Darwin (macOS, iOS, watchOS, tvOS, audioOS) |
| `linux` | Linux |
| `ohos` | OpenHarmony / HarmonyOS |
| `windows` | Microsoft Windows |
