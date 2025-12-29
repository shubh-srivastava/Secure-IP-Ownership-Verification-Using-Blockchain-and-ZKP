import { useState } from "react";

export default function VerifyIP() {
  const [creator, setCreator] = useState("");
  const [content, setContent] = useState("");
  const [msg, setMsg] = useState("");

  async function submit(e) {
    e.preventDefault();

    const res = await fetch("http://localhost:18080/verify", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ creator, content })
    });

    const data = await res.json();

    setMsg(data.valid
      ? "🟢 Ownership verified"
      : "🔴 Verification failed"
    );

    setCreator("");
    setContent(""); // ✅ clear inputs regardless
  }

  return (
    <div>
      <h2>Verify Ownership</h2>
      <form onSubmit={submit}>
        <input value={creator} onChange={e => setCreator(e.target.value)} />
        <textarea value={content} onChange={e => setContent(e.target.value)} />
        <button>Verify</button>
      </form>
      <p>{msg}</p>
    </div>
  );
}
