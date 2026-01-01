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
    setContent("");
  }

  return (
    <div className="max-w-md mx-auto mt-10 bg-white p-6 rounded shadow">
      <h2 className="text-2xl font-bold mb-4">Verify Ownership</h2>

      <form onSubmit={submit} className="flex flex-col gap-3">
        <input
          className="border p-2 rounded"
          placeholder="Creator ID"
          value={creator}
          onChange={e => setCreator(e.target.value)}
        />
        <textarea
          className="border p-2 rounded"
          placeholder="IP Content"
          value={content}
          onChange={e => setContent(e.target.value)}
        />
        <button className="bg-purple-600 text-white py-2 rounded">
          Verify
        </button>
      </form>

      {msg && <p className="mt-3">{msg}</p>}
    </div>
  );
}
