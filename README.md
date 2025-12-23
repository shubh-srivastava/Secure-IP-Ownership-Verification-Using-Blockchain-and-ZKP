# Blockchain-Based IP Ownership Verification System  
### Using Zero-Knowledge Proofs (Schnorr Protocol)

## Abstract
This project implements a **C++-based blockchain system** for registering and verifying ownership of digital intellectual property (IP) using **Zero-Knowledge Proofs (ZKP)**. The system enables users to prove ownership of registered content **without revealing private keys or original data**, ensuring privacy, integrity, and tamper resistance.

The implementation combines:
- A custom blockchain ledger
- Cryptographic hash-based IP registration
- Schnorr-style Zero-Knowledge Proofs for ownership verification
- Efficient duplicate detection and user authentication

This project is designed as a **secure, extensible prototype** suitable for academic research and real-world system evolution.

---

## Key Objectives
- Provide cryptographic proof of ownership for digital content
- Prevent duplicate IP registration across users
- Maintain a tamper-evident ledger of ownership claims
- Verify ownership without revealing sensitive information
- Demonstrate practical application of Zero-Knowledge Proofs

---

## System Architecture

### Core Components
- **Blockchain Ledger**  
  Stores immutable records of registered IP hashes linked via cryptographic hashes.
  
- **User Identity System**  
  Each user possesses a cryptographic key pair (private/public).

- **Zero-Knowledge Proof Engine**  
  Implements a Schnorr-based challenge-response protocol to prove ownership.

- **Duplicate IP Registry**  
  Ensures one-to-one mapping between content and owner.

---

## Technologies Used
- **Language**: C++
- **Cryptography**:
  - Modular Exponentiation
  - Discrete Logarithm Problem
  - Schnorr Zero-Knowledge Proof
- **Hashing**:
  - djb2 (used for IP and block hashing for performance reasons)
- **Data Structures**:
  - `vector`
  - `unordered_map`
  - Custom `struct` abstractions

---

## Project Structure

```

zkp_blockchain/
│
├── crypto.h / crypto.cpp        # Hashing and modular arithmetic
├── user.h / user.cpp            # User identity and key management
├── block.h                      # Blockchain block structure
├── blockchain.h / blockchain.cpp# Core blockchain logic and ZKP verification
└── main.cpp                     # CLI entry point

```

---

## Cryptographic Theory

### Zero-Knowledge Proof (Schnorr Protocol)
The system implements a Schnorr-style ZKP to prove knowledge of a private key `x` without revealing it.

#### Public Parameters
- Prime modulus `p`
- Generator `g`

#### User Keys
- Private key: `x`
- Public key: `y = g^x mod p`

#### Protocol Steps
1. Prover selects random `r`
2. Computes commitment `h = g^r mod p`
3. Verifier issues random challenge `c`
4. Prover responds with `s = r + c·x mod (p−1)`
5. Verification:
```

g^s ≡ h · y^c (mod p)

````

If the equality holds, ownership is verified without exposing `x`.

---

## Blockchain Design

Each block contains:
- Block index
- Hash of previous block
- Hash of IP content
- Creator ID
- Timestamp
- Block hash

Blocks are cryptographically chained, making tampering detectable.

---

## Use Cases

### Primary Use Cases
- Intellectual property ownership registration
- Academic research validation
- Digital content provenance tracking
- Secure authorship verification

### Potential Applications
- Research paper timestamping
- Music and digital art ownership claims
- Software originality verification
- Internal enterprise IP registries

---

## Setup Instructions

### Prerequisites
- C++17 compatible compiler (GCC / Clang / MSVC)
- Standard C++ library

### Compilation
```bash
g++ main.cpp blockchain.cpp crypto.cpp user.cpp -o zkp_blockchain
````

### Execution

```bash
./zkp_blockchain
```

---

## Usage Guide

### 1. Register a User

Creates a cryptographic key pair for the user.

```
Option: 1
Input: User ID
```

### 2. Register IP Content

Associates unique content with the user.

```
Option: 2
Input: User ID
Input: Content
```

Duplicate content registration by another user is rejected.

### 3. Verify Ownership

Performs Zero-Knowledge Proof verification.

```
Option: 3
Input: User ID
Input: Content
```

No private key or content hash is revealed.

### 4. View Blockchain

Displays all registered blocks with metadata.

```
Option: 4
```

---

## Security Properties

* Ownership privacy preserved
* No private keys exposed
* Tamper-evident ledger
* Replay-attack resistant ZKP
* Constant-time duplicate detection

---

## Limitations

* Uses djb2 hashing (non-cryptographic)
* In-memory storage only
* Single-node execution
* No network consensus
* No persistent storage

These limitations are intentional to keep the system lightweight and instructional.

---

## Future Enhancements

* Persistent ledger storage
* Distributed peer-to-peer nodes
* Digital signature support
* Cryptographically secure hash functions
* REST API and web interface
* Smart contract-based licensing
* zk-SNARK integration

---

## Educational Value

This project demonstrates:

* Applied blockchain engineering
* Practical Zero-Knowledge Proof usage
* Secure system design principles
* Cryptographic protocol implementation in C++
