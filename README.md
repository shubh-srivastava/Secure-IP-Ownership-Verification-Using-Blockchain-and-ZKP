# Blockchain-Based IP Ownership Verification System Using Zero-Knowledge Proofs (Schnorr Protocol)

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
* **Zero-Knowledge Proof Engine**: Implements a Schnorr-style challenge-response protocol to prove ownership.
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

## Zero-Knowledge Proof Implementation (Project Specific)

This project implements a **real Schnorr Zero-Knowledge Proof** flow using a
Fiat-Shamir (non-interactive) variant. The **prover is the client**, and the
**verifier is the server**. The server never stores private keys.

### Roles and Data Flow
- **Client (prover)** generates a private/public key pair locally.
- **Server (verifier)** stores only the public key and verifies proofs.
- The proof is bound to the IP hash and user ID so it cannot be replayed for
  other content or users.

### Steps Used in This Project
1. **Key Generation (client-side)**  
   Private key `x` is generated locally. Public key is `y = g^x mod p`.  
   The client sends `publicKey` during `/register`.
2. **Commitment (client-side)**  
   Pick random `r` and compute `R = g^r mod p`.
3. **Challenge (client-side, Fiat-Shamir)**  
   Compute `c = H(R || y || ipHash || userID) mod (p-1)` using the existing
   `djb2` hash (kept for performance constraints).
4. **Response (client-side)**  
   Compute `s = r + c * x mod (p-1)`.
5. **Verification (server-side)**  
   Server recomputes `c` and verifies:  
   `g^s = R * y^c (mod p)`

### API Payloads
- **Register User**
```json
{ "userID": "alice", "publicKey": 1234 }
```
- **Verify Ownership**
```json
{
  "creator": "alice",
  "content": "my-secret-ip",
  "commitment": 1111,
  "response": 2222
}
```

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
{ "userID": "alice", "publicKey": 1234 }
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
  "content": "my-secret-ip",
  "commitment": 1111,
  "response": 2222
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

## Frontend Implementation

The project includes a **modern web-based frontend built using React (Vite)** that interfaces directly with the C++ REST API backend. The frontend serves as a **visual and interactive layer** for demonstrating the system's blockchain and Zero-Knowledge Proof functionality without compromising cryptographic principles.

### Key Features
- **System Dashboard**  
  Displays real-time server health, blockchain status, total blocks, and genesis timestamp.
- **User Registration Interface**  
  Allows creation of cryptographic user identities via the `/register` API.
- **IP Registration Interface**  
  Enables users to register intellectual property content on the blockchain while enforcing duplicate prevention.
- **Ownership Verification Interface**  
  Demonstrates Zero-Knowledge Proof-based ownership verification through the `/verify` endpoint without exposing private keys or sensitive data.
- **Blockchain Explorer**  
  Provides a read-only, transparent view of blockchain blocks retrieved from the `/chain` endpoint.

### Frontend Design Principles
- Stateless communication with the backend via REST APIs
- Client-side key generation and local storage (keys are not sent to the server)
- Clear separation between cryptographic logic (backend) and presentation (frontend)
- Graceful handling of server availability and error states
- Responsive and clean UI styled using **Tailwind CSS**

The frontend transforms the system from a backend prototype into a **fully demonstrable cryptography application**, making it suitable for academic presentations, technical demonstrations, and future extension into a production-grade web interface.

## Setup Instructions
### Prerequisites
- Windows 10 or later  
- C++17 compatible compiler (MinGW / GCC / MSVC)  
- Node.js (v18 or later recommended)  
- npm (comes with Node.js)  
- No external C++ dependencies required (header-only libraries used)

### Backend Compilation (Windows / MinGW)

Compile the C++ REST API server using the following command:

(if running in Linux environment, remove the -lws2_32)

```bash
g++ server.cpp blockchain.cpp crypto.cpp user.cpp -std=c++17 -lws2_32 -o zkp_server
```
Run the server:
```bash
./zkp_server
```
The server will start at:
```bash
http://localhost:18080
```

## Testing With CLI Client

Compile and run the standalone CLI client to exercise the server endpoints:

```bash
g++ cli_client.cpp crypto.cpp -std=c++17 -lws2_32 -o cli_client
./cli_client
```

The CLI client can be used to confirm the server is running and supports:
- `/health` health check
- `/register` user registration with public key
- `/create` IP registration
- `/verify` ZKP ownership verification
- `/chain` blockchain export

## Frontend Setup (React + Vite)

Navigate to the frontend project directory and install dependencies:
```bash
npm install
```
Start the development server:
```bash
npm run dev 
```
The frontend will be available at:
```bash
http://localhost:5173
```
## Verification

Ensure the backend server is running before accessing the frontend.
The frontend dashboard performs a health check against the backend and displays the server status in real time.

You can manually verify the backend using:
```bash
curl http://localhost:18080/health
```
Expected response:
```json
{ "status": "ok" }
```
## Notes

- The backend must be running for frontend features such as user registration, IP registration, and ownership verification to function correctly.

- All blockchain data is stored in memory and resets when the server restarts.

- The frontend generates and stores private keys locally (browser localStorage) and communicates with the backend exclusively through REST APIs.

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
* Smart contract-based licensing
* zk-SNARK or zk-STARK integration

---

### Footnotes ###
* `user.h` and `user.cpp` are included for architectural separation and future extensibility.
* The ZKP implementation is a proof-of-concept and not production-grade.
* The `djb2` hashing algorithm is used for simplicity and performance, not cryptographic security.
