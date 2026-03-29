ZKP Fiat-Shamir Implementation Details (Project-Specific)
=========================================================

Reviewed files
--------------
- backend: blockchain.cpp, blockchain.h, crypto.cpp, server.cpp, user.h
- frontend: ip_protection/src/services/zkp.js, ip_protection/src/pages/RegisterUser.jsx,
  ip_protection/src/pages/RegisterIP.jsx, ip_protection/src/pages/VerifyIP.jsx
- auxiliary client with same proof logic: cli_client.cpp


1) Exactly how Fiat-Shamir ZKP is implemented
--------------------------------------------

Protocol style
- This is a Schnorr-style proof made non-interactive using Fiat-Shamir.
- Server never sends a random challenge. Instead, both sides derive challenge c via hash.

Public parameters
- P = 7057
- G = 5
- Defined in backend (blockchain.h) and frontend (zkp.js), so both sides use same group.

Hash and math primitives
- Hash function: djb2Hash (crypto.cpp on backend, zkp.js on frontend).
- Modular exponentiation: modExp (crypto.cpp on backend, zkp.js on frontend).

Key relation
- Private key x is client-side only.
- Public key y = G^x mod P.

Proof generation (frontend/client)
- Implemented in generateProof(userID, content) in ip_protection/src/services/zkp.js.
- Steps:
1. ipHash = djb2Hash(content)
2. r = random nonce in [0, P-2] (using crypto.getRandomValues)
3. commitment R = G^r mod P
4. challengeHash = djb2Hash( String(R) + String(y) + String(ipHash) + String(userID) )
5. challenge c = challengeHash mod (P - 1)
6. response s = (r + c*x) mod (P - 1)
- Client sends only { creator, content, commitment: R, response: s } to /verify.
- Public key is not sent at verify time; server already has it from /register.

Challenge recomputation + verification (server)
- computeChallenge(...) in blockchain.cpp uses:
  c = djb2Hash( to_string(R) + to_string(y) + to_string(ipHash) + creatorID ) mod (P - 1)
- verifyOwnership(...) in blockchain.cpp validates:
1. creatorID is registered
2. content hash exists in registry
3. content belongs to creatorID
4. Schnorr equation:
   left  = G^s mod P
   right = (R * y^c) mod P
   accept iff left == right

Important implementation note
- This is functionally a Fiat-Shamir Schnorr flow, but it uses djb2 and small parameters
  (P=7057), so it is a demo/educational implementation, not production-grade cryptography.


2) Client-side (frontend) event flow
------------------------------------

A. User registration flow (RegisterUser.jsx)
1. User enters userID and submits /register form.
2. Frontend calls createUserKeys(userID):
   - if keys already exist in localStorage ("ip_protection_keys"), reuse them
   - else generate x, compute y = G^x mod P, store {priv, pub} in localStorage
3. Frontend POSTs to /register with JSON:
   { "userID": "...", "publicKey": y }
4. Server stores userID -> publicKey if not existing.
5. Frontend shows success/failure.

B. IP registration flow (RegisterIP.jsx)
1. User enters creator and content.
2. Frontend POSTs to /create with JSON:
   { "creator": "...", "content": "..." }
3. Server hashes content, rejects duplicates, creates block if valid.
4. Frontend shows success or duplicate/failure message.

C. Ownership verification flow (VerifyIP.jsx + zkp.js)
1. User enters creator and content, clicks Verify.
2. Frontend checks local key exists via getKeys(creator); fail fast if missing.
3. Frontend generates proof via generateProof(creator, content):
   - compute R, c, s as above
4. Frontend POSTs to /verify with JSON:
   {
     "creator": "...",
     "content": "...",
     "commitment": R,
     "response": s
   }
5. Frontend reads { "valid": true/false } and renders verified/failed message.
6. Input fields are cleared after response.


3) Server-side event flow
-------------------------

Startup
1. server.cpp constructs Blockchain.
2. Blockchain tries loadState() from blockchain_state.json.
3. If load fails, initializeGenesis() creates genesis block.

Route: POST /register
1. Parse JSON; require userID + publicKey.
2. blockchain.registerUser(userID, publicKey).
3. Reject if user exists; otherwise persist state to blockchain_state.json.

Route: POST /create
1. Parse JSON; require creator + content.
2. blockchain.createBlock(creator, content):
   - require creator is registered
   - ipHash = djb2Hash(content)
   - reject if ipHash already mapped in ipRegistry
   - create new block linked with prev block hash
   - persist state

Route: POST /verify (Fiat-Shamir check path)
1. Parse JSON; require creator, content, commitment, response.
2. blockchain.verifyOwnership(creator, content, commitment, response):
   - ensure creator exists in users map
   - ipHash = djb2Hash(content)
   - ensure ipHash exists and owner matches creator
   - load public key y from users[creator]
   - recompute c from (R, y, ipHash, creator)
   - verify G^s mod P == R * y^c mod P
3. Return JSON: { "valid": true/false }.

Route: POST /reset
1. Reset chain and users to genesis-only state.
2. Persist cleared state.
3. Note: browser localStorage keys are unaffected by server reset.


4) What is and is not exposed
-----------------------------
- Exposed to server: userID, publicKey, content, commitment, response.
- Not exposed to server: private key x and nonce r.
- Stored server-side: public keys + blockchain state.
- Stored client-side (frontend): private/public keys in localStorage.
