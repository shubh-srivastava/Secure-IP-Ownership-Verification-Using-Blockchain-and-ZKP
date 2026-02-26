const P = 7057;
const G = 5;
const STORAGE_KEY = "ip_protection_keys";

function djb2Hash(input) {
  let hash = 5381;
  for (let i = 0; i < input.length; i += 1) {
    hash = ((hash << 5) + hash) + input.charCodeAt(i);
    hash >>>= 0;
  }
  return hash;
}

function modExp(base, exp, mod) {
  let result = 1n;
  let b = BigInt(base % mod);
  let e = BigInt(exp);
  const m = BigInt(mod);
  while (e > 0n) {
    if (e & 1n) result = (result * b) % m;
    b = (b * b) % m;
    e >>= 1n;
  }
  return Number(result);
}

function randomInt(maxExclusive) {
  const max = Math.max(1, maxExclusive);
  const buf = new Uint32Array(1);
  crypto.getRandomValues(buf);
  return buf[0] % max;
}

function loadKeyStore() {
  const raw = localStorage.getItem(STORAGE_KEY);
  if (!raw) return {};
  try {
    return JSON.parse(raw);
  } catch {
    return {};
  }
}

function saveKeyStore(store) {
  localStorage.setItem(STORAGE_KEY, JSON.stringify(store));
}

export function getKeys(userID) {
  const store = loadKeyStore();
  return store[userID] || null;
}

export function createUserKeys(userID) {
  const store = loadKeyStore();
  if (store[userID]) return store[userID];

  const priv = (randomInt(P - 2) + 1) | 0;
  const pub = modExp(G, priv, P);
  store[userID] = { priv, pub };
  saveKeyStore(store);
  return store[userID];
}

export function generateProof(userID, content) {
  const keys = getKeys(userID);
  if (!keys) return null;

  const ipHash = djb2Hash(content);
  const r = randomInt(P - 1);
  const commitment = modExp(G, r, P);
  const challengeHash = djb2Hash(
    String(commitment) +
    String(keys.pub) +
    String(ipHash) +
    String(userID)
  );
  const challenge = challengeHash % (P - 1);
  const response = (r + challenge * keys.priv) % (P - 1);

  return { commitment, response, publicKey: keys.pub };
}
