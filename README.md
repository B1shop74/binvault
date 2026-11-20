# BinVault

A C-based encrypted data storage application built with libsodium.

<sup>**Status: Work in Progress**</sup>

## Overview

BinVault is a command-line project designed to provide encrypted storage for user records.

The application uses authenticated encryption through libsodium and the XChaCha20-Poly1305 algorithm. It is being developed around a modular architecture that separates cryptographic operations, record management, database handling, and file configuration.

## Features & Components

- **Authenticated Encryption** — XChaCha20-Poly1305 through libsodium.
- **Key Management** — cryptographic key and salt handling based on a master password.
- **Encrypted Records** — individual user fields such as names, passwords, and additional information are represented in encrypted form.
- **Custom File Format** — application-specific binary format identified by the `BVLT` header (version 1).
- **Database Management** — structures and operations for managing encrypted user records.
- **File Management** — database directory and file configuration.
- **Logging** — a separate module for application logging.

*Database operations and the command-line interface are still under development.*

## Technologies

- C
- libsodium
- XChaCha20-Poly1305
- Make

## Project Structure

```text
BinVault/
├── main.c
├── crypto.c
├── crypto.h
├── database.c
├── database.h
├── record.c
├── record.h
├── fileConfig.c
├── fileConfig.h
├── logger.c
├── logger.h
├── Makefile
└── README.md
```

## Architecture

| Module | Responsibility |
|---|---|
| `crypto` | Encryption, decryption, and key management |
| `database` | Database structures and encrypted record management |
| `record` | User record structure and field handling |
| `fileConfig` | Database directory and file operations |
| `logger` | Application logging |
| `main` | Application entry point and command-line interaction |

## Cryptography

BinVault uses **XChaCha20-Poly1305-IETF**, an authenticated encryption algorithm provided by libsodium.

The cryptographic module is designed to handle:

- Master-password-based key initialization.
- Salt management.
- Encryption and decryption of individual record fields.
- Nonce handling for encrypted fields.
- Encryption and decryption of complete user records.

The project also uses a custom binary file format identified by the `BVLT` header.

## Development Status

BinVault is an ongoing personal systems programming project.

The cryptographic and record-management architecture has been developed, while database operations and the main application interface remain unfinished.

Further development will focus on:

- Completing database operations.
- Finalizing the command-line interface.
- Integrating all modules.
- Testing data storage and retrieval.
- Improving error handling and overall reliability.

## Security Notice

BinVault is an experimental project and is not intended for production use.

Its security implementation and overall application behavior have not yet undergone a comprehensive security audit. Do not use it to store real credentials or sensitive information.

## Author

**B1shop74**

GitHub: https://github.com/B1shop74
