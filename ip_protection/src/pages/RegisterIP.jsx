import { useState } from "react";

export default function RegisterIP() {
  const [creator, setCreator] = useState("");
  const [content, setContent] = useState("");
  const [msg, setMsg] = useState("");

  async function submit(e) {
    e.preventDefault();

    const res = await fetch("http://localhost:18080/create", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ creator, content })
    });

    const data = await res.json();

    if (data.success) {
      setMsg("✅ IP registered successfully");
      setCreator("");
      setContent(""); // ✅ clear both
    } else {
      setMsg("❌ Duplicate IP detected");
    }
  }

  return (
    <div>
      <h2>Register IP</h2>
      <form onSubmit={submit}>
        <input value={creator} onChange={e => setCreator(e.target.value)} />
        <textarea value={content} onChange={e => setContent(e.target.value)} />
        <button>Submit</button>
      </form>
      <p>{msg}</p>
    </div>
  );
}
