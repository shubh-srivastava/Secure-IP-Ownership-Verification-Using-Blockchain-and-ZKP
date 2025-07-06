# 🧠 Blockchain-Based IP Ownership Verification System with Zero-Knowledge Proof (ZKP)

This project is a C++ implementation of a simplified **Blockchain** integrated with **Zero-Knowledge Proof (ZKP)** to securely register, track, and verify ownership of digital content (represented as IP data hashes). The system ensures that content is only registered once and can be cryptographically verified without revealing the original data.

## 📌 Features

- ✅ **Blockchain Ledger**: Custom block structure storing hash of content, timestamp, creator ID, and links to previous blocks.
- 🔐 **Zero-Knowledge Proof (ZKP)**: Verifies a user's ownership of IP data without exposing the actual hash using modular exponentiation and challenge-response protocol.
- ⚠️ **Duplicate Detection**: Prevents registration of the same content by different users.
- 🧾 **Transaction History**: All blocks are viewable with timestamped metadata.
- 🖥️ **Command-Line Interface (CLI)**: Interactive interface to create blocks, verify transactions, and inspect the blockchain.

## 🛠️ Technologies Used

- **Language**: C++
- **Hash Function**: `djb2` (for IP content hashing)
- **Cryptography**: Modular Exponentiation (Discrete Logarithm Problem)
- **Data Structures**: `vector`, `unordered_map`, and custom `struct`

## 🔄 How It Works

1. **Creating a Block**:
   - User provides their ID and content.
   - The system hashes the content using `djb2` and checks if it's already been registered.
   - If valid, a block is created and linked to the previous one.

2. **Verifying Ownership**:
   - A ZKP protocol is used:
     - Random `r` is chosen.
     - Commitment `h = g^r mod p` is computed.
     - Challenge `b` is generated.
     - Response `s = (r + b*x) mod (p-1)` is calculated, where `x` is the secret (the IP hash).
     - Verifier checks `g^s ≡ h * y^b (mod p)` where `y = g^x`.

3. **Viewing Transactions**:
   - All blocks with creator ID, hash, and timestamp can be displayed.

## 📸 Sample CLI Output

```bash
Enter 1 to create a new block
Enter 2 to verify a transaction
Enter 3 to view all transactions/blocks
Enter 4 to exit
