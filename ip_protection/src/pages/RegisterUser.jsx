import { useState } from "react";

export default function RegisterUser() {
  const [userID, setUserID] = useState("");
  const [msg, setMsg] = useState("");

  async function submit(e) {
    e.preventDefault();

    const res = await fetch("http://localhost:18080/register", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ userID })
    });

    const data = await res.json();

    if (data.success) {
      setMsg("✅ User registered successfully");
      setUserID(""); // ✅ clear input
    } else {
      setMsg("❌ User already exists");
    }
  }

  return (
    <div>
      <h2>Register User</h2>
      <form onSubmit={submit}>
        <input
          value={userID}
          onChange={e => setUserID(e.target.value)}
        />
        <button>Register</button>
      </form>
      <p>{msg}</p>
    </div>
  );
}
