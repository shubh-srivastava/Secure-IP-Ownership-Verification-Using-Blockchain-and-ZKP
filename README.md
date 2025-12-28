# Blockchain-Based IP Ownership Verification System
### Using Zero-Knowledge Proofs (Schnorr Protocol)

---

## Abstract

This project implements a **C++-based blockchain-backed system** for registering and verifying ownership of digital intellectual property (IP) using **Zero-Knowledge Proofs (ZKP)**. The system allows users to prove ownership of registered content **without revealing private keys or original content**, ensuring privacy, integrity, and tamper resistance.

The current implementation extends beyond a CLI prototype and provides a **fully functional REST API backend**, enabling programmatic interaction, JSON-based blockchain export, and seamless integration with future frontends.

This project is designed as a **secure, extensible prototype** suitable for academic research, cryptography demonstrations, and real-world system evolution.

---

## Key Objectives

* Provide cryptographic proof of ownership for digital content
* Prevent duplicate IP registration across users
* Maintain a tamper-evident blockchain ledger
* Verify ownership without revealing sensitive information
* Demonstrate practical application of Zero-Knowledge Proofs
* Expose system functionality via a RESTful API

---

## System Architecture

### Core Components

* **Blockchain Ledger**: Stores immutable records of registered IP hashes, cryptographically linked via block hashes.
* **User Identity System**: Each user is assigned a cryptographic key pair (private/public).
* **Zero-Knowledge Proof Engine**: Implements a Schnorr-style challenge–response protocol to prove ownership.
* **Duplicate IP Registry**: Enforces one-to-one mapping between content and owner.
* **REST API Server**: Exposes blockchain functionality over HTTP using JSON payloads.

---
## Cryptographic Theory

### Zero-Knowledge Proof (Schnorr Protocol)

The system implements a Schnorr-style Zero-Knowledge Proof to prove knowledge of a private key $x$ without revealing it.

#### Public Parameters
* Prime modulus $p$
* Generator $g$

#### User Keys
* **Private key**: $x$
* **Public key**: $y = g^x \pmod p$

#### Protocol Steps
1. **Commitment**: Prover selects random $r$ and computes $h = g^r \pmod p$.
2. **Challenge**: Verifier issues a random challenge $c$.
3. **Response**: Prover responds with $s = r + c \cdot x \pmod{p-1}$.
4. **Verification Condition**:
   $$g^s \equiv h \cdot y^c \pmod p$$

If the equality holds, ownership is verified without exposing $x$.

## Blockchain Design

Each block contains:
* Block index
* Hash of previous block
* Hash of IP content
* Creator ID
* Timestamp
* Block hash

Blocks are cryptographically chained, making tampering immediately detectable.

---

## REST API Endpoints

### Health Check
`GET /health`
```json
{ "status": "ok" }
```
### Register User
`POST /register Request:`
``` JSON
{ "userID": "alice" }
```
### Response:
``` JSON
{ "success": true }
```
### Register IP Content
`POST /create` Request:
``` JSON
{
  "creator": "alice",
  "content": "my-secret-ip"
}
```
### Response:
``` JSON
{ "success": true }
```
Note: Duplicate content registration by another user is rejected.

### Verify Ownership
`POST /verify` Request:
``` JSON
{
  "creator": "alice",
  "content": "my-secret-ip"
}
```
### Response:
``` JSON
{ "valid": true }
```
### Export Blockchain
### `GET /chain` Response:
``` JSON
[
  {
    "index": 0,
    "creator": "system",
    "ipHash": 0,
    "prevHash": 0,
    "blockHash": 0,
    "timestamp": 1766945885
  }
]
```
## Setup Instructions

### Prerequisites
* Windows 10 or later
* C++17 compatible compiler (MinGW / GCC / MSVC)
* No external dependencies required (header-only libraries used)

### Compilation (Windows / MinGW)
```bash
g++ server.cpp blockchain.cpp crypto.cpp user.cpp -std=c++17 -lws2_32 -o zkp_server
```
## Security Properties
* Ownership privacy preserved
* No private keys exposed
* Tamper-evident blockchain ledger
* Replay-resistant ZKP protocol
* Constant-time duplicate detection
* Read-only blockchain export

## Limitations
* Uses `djb2` hashing (non-cryptographic)
* In-memory storage only
* Single-node execution
* No persistent storage
* No authentication tokens
* Small prime numbers used for ZKP (educational purposes)

## Future Enhancements
* Persistent blockchain and user storage
* Cryptographically secure hash functions (SHA-256)
* Digital signatures for request authentication
* Distributed peer-to-peer consensus
* Web-based frontend (React)
* Smart contract-based licensing
* zk-SNARK or zk-STARK integration

---

**Footnotes**
* `user.h` and `user.cpp` are included for architectural separation and future extensibility.
* The ZKP implementation is a proof-of-concept and not production-grade.
* The `djb2` hashing algorithm is used for simplicity and performance, not cryptographic security.