# Firmware Notes

## Memory Layout

The application firmware is designed to run from:

```text
0x08011000
```

The APP startup code sets:

```c
SCB->VTOR = 0x08011000;
```

The BootLoader reads the initial stack pointer and reset vector from the same address, then jumps to the application.

## Boot Flow

```text
Power on / reset
    ↓
BootLoader starts
    ↓
Validate APP stack address
    ↓
Disable interrupts and SysTick
    ↓
Set MSP
    ↓
Set vector table offset
    ↓
Jump to APP reset handler
```

## APP Main Modules

| Module | Path | Responsibility |
|---|---|---|
| Main control | `firmware/app/User/main.c` | Initialization, command dispatch, sampling control |
| Protocol | `firmware/app/Protocol/Protocol.c` | ASCII hex frame parsing and CRC check |
| ADC | `firmware/app/Driver/ADC.c` | Internal ADC sampling |
| DAC | `firmware/app/Driver/DAC.c` | DAC output |
| AD3344 | `firmware/app/Driver/SPI_AD3344.c` | External ADC configuration and sampling |
| SPI Flash | `firmware/app/Driver/SPI_FLASH.c` | Parameter storage |
| OLED | `firmware/app/Driver/OLED/oled.c` | Local display |
| USART | `firmware/app/Driver/USART.c` | Serial communication |

## Protocol Frame

The protocol uses ASCII hex encoding.

```text
A5B6 [device_id] [frame_type] [command] [data_len] [data] [crc] B6A5
```

Implementation details:

- Frame header: `A5B6`
- Frame tail: `B6A5`
- CRC: CRC-16-Modbus
- Parser: `parse_received_frame`
- Error response path is handled when CRC/tail validation fails.

## Build Outputs

Prebuilt hex files are stored in:

```text
binaries/app_project.hex
binaries/bootloader_project.hex
```

Intermediate build artifacts are intentionally excluded from the repository.
